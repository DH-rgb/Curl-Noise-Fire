#version 120
//
// simple.vert
//
attribute vec4 point;//位置+id
attribute vec4 value;//速度+ライフ
uniform mat4 transformMatrix;//変換行列
uniform float elapsedTime;//実行時間
varying vec4 position;//pointの格納先
varying vec4 next;//valueの格納先
const float speed = 0.3;//係数
const vec3 accel = vec3(0.0,0.04,0.0); //上昇ベクトル

varying float lifeColor;//カラー用ライフ
const float pi = 3.1415926535;
const float dtime = 0.5;
const int noise_strength = 3;

const float delta = 5.0/700;
const vec3 distination = vec3(0.0,500.0,0.0);

//シンプレックスノイズ生成

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

vec3 snoiseVec3( vec3 x ){

  float s  = snoise(vec3( x ));
  float s1 = snoise(vec3( x.y - 19.1 , x.z + 33.4 , x.x + 47.2 ));
  float s2 = snoise(vec3( x.z + 74.2 , x.x - 124.5 , x.y + 99.4 ));
  vec3 c = vec3( s , s1 , s2 );
  return c;

}

//カールノイズ生成

vec3 curlNoise( vec3 p ){
  
  const float e = .1;
  vec3 dx = vec3( e   , 0.0 , 0.0 );
  vec3 dy = vec3( 0.0 , e   , 0.0 );
  vec3 dz = vec3( 0.0 , 0.0 , e   );

  vec3 p_x0 = snoiseVec3( p - dx );
  vec3 p_x1 = snoiseVec3( p + dx );
  vec3 p_y0 = snoiseVec3( p - dy );
  vec3 p_y1 = snoiseVec3( p + dy );
  vec3 p_z0 = snoiseVec3( p - dz );
  vec3 p_z1 = snoiseVec3( p + dz );

  float x = p_y1.z - p_y0.z - p_z1.y + p_z0.y;
  float y = p_z1.x - p_z0.x - p_x1.z + p_x0.z;
  float z = p_x1.y - p_x0.y - p_y1.x + p_y0.x;

  const float divisor = 1.0 / ( 2.0 * e );
  return normalize( vec3( x , y , z ) * divisor );

}

//カラー用のライフ値計算

float calc_life(float height){
  float a = 0.4/80000.0;
  float b = 0.2/400.0;
  return 1.0-(a*height*height+b*height) ;
}

 
void main(void)
{
  
  gl_Position = transformMatrix * vec4(point.xyz, 1.0);

  //カールノイズ生成
  //vec3 v = vec3(0);//検証用の０カールノイズ
  vec3 v = curlNoise(point.xyz/100);


  vec3 dis = point.xyz;//位置格納
  vec3 vector = value.xyz;//速度格納
  float life = value.w;//ライフ格納
  float fate = rand(point.xy)/200.0; //ライフを減らす量を決める
  float colorlife = calc_life(dis.y);//ライフを高さを元に関数から算出
  float away = length(dis.xz) + 1.0; //半径距離

  if(life < 0.05){//再発生条件
    float r = 70.0f * rand(dis.xy) ;
    float theta = pi * 2.0f * rand(dis.yz) ;
    float phi = pi *rand(dis.xz) ;
    dis.x = r * sin(theta) * cos(phi);
    dis.y=  -r * sin(phi);
    dis.z= r * cos(theta) * cos(phi);
    vector = vec3(0.0, 0.0, 0.0);
    life = 1.0;
  }
  lifeColor = colorlife;//フラグメントシェーダーにカラー用ライフ値を送る
  float vel = 1.0 - delta*away;//上昇気流減少率(中心からの距離)

  if(elapsedTime<dtime*point.w){//idによって最初に上昇タイミングをずらす。要調整
    position = vec4(dis,point.w);;
    next = vec4(vector.xyz,life);
  }
  /*else{//検証用円筒炎}
    vec3 n = normalize(vec3(0.0-dis.x,0.0,0.0-dis.z))*0.5;//中心方向へ向かうベクトル
    position = vec4(dis+ v*4*speed+ (accel+n) * speed+ vector.xyz * speed,point.w);//vec4(dis+ v*4*speed+ accel * dis.y/200.0 * speed *vel + vector.xyz * speed,point.w); // 点の位置を移動する
    next = vec4(accel*speed + vector.xyz,life-fate);
  }*/
 //以下、上昇気流accelには高さと中心位置を加味した係数をかけています

  else if(mod(point.w,2.0)==0){//idが偶数。中心に向かうベクトルを追加
    //指向性上昇気流を求める
    vec3 n = normalize(vec3(0.0-dis.x,0.0,0.0-dis.z))*0.5;//中心方向へ向かうベクトル
    position = vec4(dis + v*speed + (accel+n)* dis.y/200.0 * speed *vel+ vector.xyz * speed,point.w);//中心に向かう上昇気流にのせる
    next = vec4(accel* speed+ vector.xyz,life-fate);
  }
  else{//高さによってcurlnoiseの強度を変える。idが奇数のもの
  //curl noiseの強さを高さによって段階的に調整しています
    position = vec4(dis+ (1+step(20,dis.y)*noise_strength+step(400,dis.y)*noise_strength)*v*speed+ accel * dis.y/200.0 * speed *vel + vector.xyz * speed,point.w); // 点の位置を移動する
    next = vec4(accel* speed + vector.xyz,life-fate);
  }
}
