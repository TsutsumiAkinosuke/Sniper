//スナイプ機構リミットスイッチ
int ls_snipe = PD2;

//スナイプ機構横リミットスイッチ
int ls_snipe_hr_R = PC11;
int ls_snipe_hr_L = PC10;

//スナイプ機構縦リミットスイッチ
int ls_snipe_vt_U = PC12;
int ls_snipe_vt_D = PA13;

//壁の穴タクトスイッチ
int sw_wall_A = PA14;
int sw_wall_B = PA15;
int sw_wall_C = PB7;
int sw_wall_D = PC13;
int sw_wall_E = PC2;
int sw_wall_F = PC3;

//操作スイッチ
int sw_hr = PA10;
int sw_led_hr = PB3;
int sw_vt = PB5;
int sw_led_vt = PB4;

//リニアモジュール
int mt_linear_A = PC5;
int mt_linear_B = PC6;
int mt_linear_P = PC8;

//移動機構
int mt_hr_A = PB2;
int mt_hr_B = PB1;
int mt_hr_P = PB15;
int mt_vt_A = PC4;
int mt_vt_B = PB13;
int mt_vt_P = PB14;

int oneplay_button = PA5;
int pre_oneplaybutton = HIGH;

//移動スピード(0~255)
int spd_hr = 250;
int spd_vt = 250;
int spd_snipe = 250;
int shrink_time = 3000;

long shift_time = random(50,151);

unsigned long c_time = 0;
unsigned long vt_time = 0;
unsigned long hr_time = 0;
unsigned long w_time = 0;

void setup() {

  //シリアル通信の初期化
  Serial.begin(115200);
  
  //スイッチ系統の初期化
  pinMode(ls_snipe,INPUT_PULLUP);
  pinMode(ls_snipe_hr_R,INPUT_PULLUP);
  pinMode(ls_snipe_hr_L,INPUT_PULLUP);
  pinMode(ls_snipe_vt_U,INPUT_PULLUP);
  pinMode(ls_snipe_vt_D,INPUT_PULLUP);
  pinMode(sw_wall_A,INPUT_PULLUP);
  pinMode(sw_wall_B,INPUT_PULLUP);
  pinMode(sw_wall_C,INPUT_PULLUP);
  pinMode(sw_wall_D,INPUT_PULLUP);
  pinMode(sw_wall_E,INPUT_PULLUP);
  pinMode(sw_wall_F,INPUT_PULLUP);
  pinMode(sw_hr,INPUT_PULLUP);
  pinMode(sw_vt,INPUT_PULLUP);
  pinMode(sw_led_hr,OUTPUT);
  pinMode(sw_led_vt,OUTPUT);

  pinMode(oneplay_button,INPUT_PULLUP);

  //モータ系統の初期化
  pinMode(mt_linear_A,OUTPUT);
  pinMode(mt_linear_B,OUTPUT);
  pinMode(mt_vt_A,OUTPUT);
  pinMode(mt_vt_B,OUTPUT);
  pinMode(mt_hr_A,OUTPUT);
  pinMode(mt_hr_B,OUTPUT);
}

int wallsw_is_pushed(){
  int sw_list[6] = {sw_wall_D,sw_wall_A,sw_wall_E,sw_wall_B,sw_wall_F,sw_wall_C};
  for(int i = 0; i < 6; i++){
    if(digitalRead(sw_list[i]) == LOW){
      return i+1;
    }
  }
  return 0;
}

int game_mode = 0;
/*
 * 0:待機状態
 * 1:1回プレイ
 * 2:2回プレイ
 * 3:3回プレイ
 * 4:デバッグモード
 */

int game_state = 0;
/*
 * 0:予備
 * 1:上昇操作待機状態
 * 2:上昇中
 * 3:右操作待機状態
 * 4:右へ移動中
 * 5:スナイプ中
 * 6:リニアモジュールを縮める
 * 7:スタート地点に戻る
 * 8:終了処理
 */

void idle_sequence(){
  //縦と横の操作スイッチのLEDを消灯
  digitalWrite(sw_led_vt,LOW);
  digitalWrite(sw_led_hr,LOW);
  
  //mt_hrとmt_vtを回してスタート地点に戻る
  if(digitalRead(ls_snipe_vt_D) == HIGH){
    digitalWrite(mt_vt_A,LOW);
    digitalWrite(mt_vt_B,HIGH);
    analogWrite(mt_vt_P,spd_vt);
  }else{
    digitalWrite(mt_vt_A,LOW);
    digitalWrite(mt_vt_B,LOW);
    analogWrite(mt_vt_P,0);
  }
      
  if(digitalRead(ls_snipe_hr_L) == HIGH){
    digitalWrite(mt_hr_A,LOW);
    digitalWrite(mt_hr_B,HIGH);
    analogWrite(mt_hr_P,spd_hr);
  }else{
    digitalWrite(mt_hr_A,LOW);
    digitalWrite(mt_hr_B,LOW);
    analogWrite(mt_hr_P,0);
  }
}

void game_sequence(){
  switch(game_state){
    case 0:
      //mt_hrとmt_vtを回してスタート地点に戻る
      if(digitalRead(ls_snipe_vt_D) == HIGH){
        digitalWrite(mt_vt_A,LOW);
        digitalWrite(mt_vt_B,HIGH);
        analogWrite(mt_vt_P,spd_vt);
      }else{
        digitalWrite(mt_vt_A,LOW);
        digitalWrite(mt_vt_B,LOW);
        analogWrite(mt_vt_P,0);
      }
      
      if(digitalRead(ls_snipe_hr_L) == HIGH){
        digitalWrite(mt_hr_A,LOW);
        digitalWrite(mt_hr_B,HIGH);
        analogWrite(mt_hr_P,spd_hr);
      }else{
        digitalWrite(mt_hr_A,LOW);
        digitalWrite(mt_hr_B,LOW);
        analogWrite(mt_hr_P,0);
      }

      if(digitalRead(ls_snipe_vt_D) == LOW && digitalRead(ls_snipe_hr_L) == LOW){
        game_state += 1;
        vt_time = millis();
      }
      break;
    case 1:
      //sw_vtのLEDを点滅
      if(((c_time - vt_time)/1000)%2==0){
        digitalWrite(sw_led_vt,HIGH);
      }else{
        digitalWrite(sw_led_vt,LOW);
      }
      //sw_vtが押されるのを待つ
      if(digitalRead(sw_vt) == LOW){
        game_state += 1;
      }
      break;
    case 2:
      //sw_vtが押されていてls_snipe_vt_Uが押されていない間mt_vtを回す
      if(digitalRead(sw_vt) == LOW && digitalRead(ls_snipe_vt_U) == HIGH){
        //sw_vtのLEDを点灯
        digitalWrite(sw_led_vt,HIGH);
        //mt_vtを回す
        digitalWrite(mt_vt_A,HIGH);
        digitalWrite(mt_vt_B,LOW);
        analogWrite(mt_vt_P,spd_vt);
      }else{
        //sw_vtのLEDを消灯
        digitalWrite(sw_led_vt,LOW);
        //少しだけ滑らせる
        w_time = millis();
        shift_time = random(100,171);
        while(digitalRead(ls_snipe_vt_U) == HIGH && c_time-w_time < shift_time){
          c_time = millis();
        }
        //mt_vtを止める
        digitalWrite(mt_vt_A,LOW);
        digitalWrite(mt_vt_B,LOW);
        analogWrite(mt_vt_P,0);
        hr_time = millis();
        game_state += 1;
      }
      break;
    case 3:
      //sw_hrのLEDを点灯
      if(((c_time - hr_time)/1000)%2==0){
        digitalWrite(sw_led_hr,HIGH);
      }else{
        digitalWrite(sw_led_hr,LOW);
      }
      //sw_hrが押されるのを待つ
      if(digitalRead(sw_hr) == LOW){
        game_state += 1;
      }
      break;
    case 4:
      //sw_hrが押されているかls_snipe_hr_Rが押されていない間mt_hrを回す
      if(digitalRead(sw_hr) == LOW && digitalRead(ls_snipe_hr_R) == HIGH){
        //sw_hrのLEDを点灯
        digitalWrite(sw_led_hr,HIGH);
        //mt_hrを回す
        digitalWrite(mt_hr_A,HIGH);
        digitalWrite(mt_hr_B,LOW);
        analogWrite(mt_hr_P,spd_hr);
      }else{
        //sw_hrのLEDを消灯
        digitalWrite(sw_led_hr,LOW);
        //少しだけ滑らせる
        w_time = millis();
        shift_time = random(100,171);
        while(digitalRead(ls_snipe_hr_R) == HIGH && c_time-w_time < shift_time){
          c_time = millis();
        }   
        //mt_hrを止める
        digitalWrite(mt_hr_A,LOW);
        digitalWrite(mt_hr_B,LOW);
        analogWrite(mt_hr_P,0);
        game_state += 1;
      }
      break;
    case 5:
      //sw_hrが押されなくなったらmt_linearを回してリニアモジュールを伸ばす
      //ls_snipeが押されたときsw_wallが押されていたら演出
      if(digitalRead(ls_snipe) == HIGH){
        digitalWrite(mt_linear_A,HIGH);
        digitalWrite(mt_linear_B,LOW);
        analogWrite(mt_linear_P,spd_snipe);
      }else{
        digitalWrite(mt_linear_A,LOW);
        digitalWrite(mt_linear_B,LOW);
        analogWrite(mt_linear_P,0);
        //壁のスイッチが押されていたら
        int pushed_sw = wallsw_is_pushed();
        if(pushed_sw != 0){
          shrink_time = 4200;
          Serial.print(pushed_sw);
        }else{
          shrink_time = 2700;
        }
        delay(1100);
        game_state += 1;
      }
      break;
    case 6:
      //mt_linearを逆回転してリニアモジュールを縮める
      digitalWrite(mt_linear_A,LOW);
      digitalWrite(mt_linear_B,HIGH);
      analogWrite(mt_linear_P,spd_snipe);
      delay(shrink_time);  //遅延時間は要調整
      digitalWrite(mt_linear_A,LOW);
      digitalWrite(mt_linear_B,LOW);
      analogWrite(mt_linear_P,0);
      game_state += 1;
      break;
    case 7:
      //mt_hrとmt_vtを回してスタート地点に戻る
      if(digitalRead(ls_snipe_vt_D) == HIGH){
        digitalWrite(mt_vt_A,LOW);
        digitalWrite(mt_vt_B,HIGH);
        analogWrite(mt_vt_P,spd_vt);
      }else{
        digitalWrite(mt_vt_A,LOW);
        digitalWrite(mt_vt_B,LOW);
        analogWrite(mt_vt_P,0);
      }
      
      if(digitalRead(ls_snipe_hr_L) == HIGH){
        digitalWrite(mt_hr_A,LOW);
        digitalWrite(mt_hr_B,HIGH);
        analogWrite(mt_hr_P,spd_hr);
      }else{
        digitalWrite(mt_hr_A,LOW);
        digitalWrite(mt_hr_B,LOW);
        analogWrite(mt_hr_P,0);
      }

      if(digitalRead(ls_snipe_vt_D) == LOW && digitalRead(ls_snipe_hr_L) == LOW){
        game_state += 1;
      }
      break;
    case 8:
      game_mode -= 1;
      game_state = 0;
      break;
  }
}

void get_mode(){
  //バッファが空になるまでループして読み込む
  while(Serial.available()){
    byte temp = Serial.read();
    if(temp-48 == 0){
      game_mode = 0;
      game_state = 0;
    }else{
      game_mode += (temp-48);
    }
  }
}

void loop() {
  c_time = millis();
  
  get_mode();
  if(game_mode >= 1){
    game_sequence();
  }else{
    idle_sequence();
  }

  if(digitalRead(oneplay_button) == LOW && pre_oneplaybutton == HIGH){
    game_mode += 1;
  }
  pre_oneplaybutton = digitalRead(oneplay_button);
  
  //String msg = String(game_mode) + String("\t") + String(game_state);
  //String msg = String(digitalRead(sw_vt),DEC) + String(digitalRead(sw_hr),DEC);
  //String msg = String(digitalRead(ls_snipe_vt_U));
  //Serial.println(msg);
  delay(20);
}
