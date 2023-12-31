//#include "lv_demo_widgets.h"
#include <lvgl.h>
#include <demos/lv_demos.h>
#include <examples/lv_examples.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C  u8g2(U8G2_R0, /* clock=*/ 20, /* data=*/ 19, /* reset=*/ U8X8_PIN_NONE);     // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED

/*******************************************************************************

 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#define TFT_BL 2
//#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *lcd = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

//Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
//    GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
//    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
//    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
//    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4/* G5 */,
//    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */
//);
Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
  41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 0 /* PCLK */,
  14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
  9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
  15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */
);

// option 1:
// 7inch 50PIN 800*480
Arduino_RPi_DPI_RGBPanel *lcd = new Arduino_RPi_DPI_RGBPanel(
  bus,
  //  800 /* width */, 0 /* hsync_polarity */, 8/* hsync_front_porch */, 2 /* hsync_pulse_width */, 43/* hsync_back_porch */,
  //  480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 2/* vsync_pulse_width */, 12 /* vsync_back_porch */,
  //  1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);

  //    800 /* width */, 0 /* hsync_polarity */, 210 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
  //    480 /* height */, 0 /* vsync_polarity */, 22 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */,
  //    1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);
  800 /* width */, 0 /* hsync_polarity */, 40 /* hsync_front_porch */, 48 /* hsync_pulse_width */, 40 /* hsync_back_porch */,
  480 /* height */, 0 /* vsync_polarity */, 13 /* vsync_front_porch */, 1 /* vsync_pulse_width */, 31 /* vsync_back_porch */,
  1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
   End of Arduino_GFX setting
 ******************************************************************************/
#include <EEPROM.h>
String e_flag = "B"; //If you want to modify the saved WIFI information, just change this letter
#include <WiFi.h>
#include <Ticker.h>          //Call the ticker. H Library
Ticker ticker1;
#include "WiFiMulti.h"
WiFiMulti wifiMulti;
#include "BLEDevice.h"              //BLE Driver Library
#include "BLEServer.h"              //BLE Bluetooth Server Library
#include "BLEUtils.h"               //BLE Utility Library
#include "BLE2902.h"                //Feature Addition Descriptor Library
#include <BLECharacteristic.h>      //BLE Feature Function Library
BLEAdvertising* pAdvertising = NULL;
BLEServer* pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic* pCharacteristic = NULL;
#define bleServerName "Wizee-Series-7.0"  //Name of the BLE server
#define SERVICE_UUID "6479571c-2e6d-4b34-abe9-c35116712345"  //UUID of the service
#define CHARACTERISTIC_UUID "826f072d-f87c-4ae6-a416-6ffdcaa02d73"
bool connected_state = false;   //Creating a Device Connection Identifier

class MyServerCallbacks: public BLEServerCallbacks  //Create connect and disconnect calling classes
{
    void onConnect(BLEServer *pServer)//Start connection function
    {
      connected_state = true;   //Equipment is properly connected
    }
    void onDisconnect(BLEServer *pServer)//Disconnect Function
    {
      connected_state = false;  //Device connection error
    }

};

char CloseData;
int NO_Test_Flag = 0;
int Test_Flag = 0;
int Close_Flag = 0;

#define SERIAL_BAUD 9600
HardwareSerial cardSerial(1);//Declare serial port 1

const char *ssid = "elecrow888"; //Your network name
const char *password = "elecrow2014"; //Your network password
char buf[100] = {};
int bufindex = 0;
int wifi_close_flag = 0;
char *info[100] = {};
int wifi_flag = 0;
int i = 0;
int home_flag = 0;
int touch_flag = 0;

//7.0
#define SD_MOSI 11
#define SD_MISO 13
#define SD_SCK 12
#define SD_CS 10
unsigned long previousBlinkTime;
int blinkInterval = 15000; //LED blinking time interval
#define I2S_DOUT      17
#define I2S_BCLK      42
#define I2S_LRC       18
#define BUTTON_PIN    38

//UI
#include <ui.h>
static int first_flag = 0;
extern int zero_clean;
extern int goto_widget_flag;
extern int bar_flag;
extern lv_obj_t * ui_MENU;
extern lv_obj_t * ui_TOUCH;
extern lv_obj_t * ui_JIAOZHUN;
extern lv_obj_t * ui_Label2;
static lv_obj_t * ui_Label;//TOUCH interface label
static lv_obj_t * ui_Label3;//TOUCH interface label3
static lv_obj_t * ui_Labe2;//Menu interface progress bar label
static lv_obj_t * bar;//Menu interface progress bar
SPIClass& spi = SPI;
uint16_t touchCalibration_x0 = 300, touchCalibration_x1 = 3600, touchCalibration_y0 = 300, touchCalibration_y1 = 3600;
uint8_t  touchCalibration_rotate = 1, touchCalibration_invert_x = 2, touchCalibration_invert_y = 0;
static int val = 100;
/*******************************************************************************
   Please config the touch panel in touch.h
 ******************************************************************************/
#include "touch.h"
#include "Audio.h"
Audio audio;
//String music_url;
//int music_id = 1818615872;


/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
//static lv_color_t *disp_draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 15];
static lv_disp_drv_t disp_drv;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  lcd->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  lcd->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
      Serial.print( "Data x " );
      Serial.println( data->point.x );
      Serial.print( "Data y " );
      Serial.println( data->point.y );
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
}

//Read or Save infomation；
void save_or_read_wifi(void) {
  EEPROM.begin(1024);
  if (get_string(1, 0) == e_flag) { //If the 0th character is "M", the WIFI information is stored.
    Serial.println("Stored information");
  }
  else {
    Serial.println("No WIFI information is stored, wifi information will be stored.");
    //Save infomation
    set_string(1, 0, e_flag, 0); //Entering the "N" character
    Serial.println("Completion of storage");
    Serial.println("It's about to be relaunched.");
    ESP.restart();
  }
  EEPROM.end();
}

//Use the a bit of EEPROM to save the length of the string, the string is saved from the b bit of EEPROM, str is the string to be saved, and s is whether to save the length of the string.
void set_string(int a, int b, String str, int s)
{
  if (s)EEPROM.write(a, str.length()); //EEPROM bit a, write length of str string
  //Save all str data, one by one, in the EEPROM via a for loop.
  for (int i = 0; i < str.length(); i++) {
    EEPROM.write(b + i, str[i]);
  }
  EEPROM.commit();  //Execute Save EEPROM

}

//Get the string of the specified EEPROM location, a is the length of the string, b is the start bit, start reading from bit b of the EEPROM
String get_string(int a, int b) {
  String data = "";
  //With a for loop, the value of each bit is taken out of the EEPROM one by one and linked together
  for (int i = 0; i < a; i++) {
    data += char(EEPROM.read(b + i));
  }
  return data;
}

void callback1()  //Callback function
{
  if (bar_flag == 6)
  {
    if (val > 1)
    {
      val--;
      lv_bar_set_value(bar, val, LV_ANIM_OFF);
      lv_label_set_text_fmt(ui_Labe2, "%d %%", val);
    }
    else
    {
      lv_label_set_text(ui_Labe2, "Loading");
      delay(150);
      val = 100;
      bar_flag = 0; //Stop progress bar sign
      goto_widget_flag = 1; //Widget Enter logo

    }
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.println("LVGL Widgets Demo");

  //IO Port Pins
  pinMode(38, OUTPUT);
  digitalWrite(38, LOW);
  pinMode(17, OUTPUT);
  digitalWrite(17, LOW);
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
//  EEPROM.begin(1024);
//  if (get_string(1, 0) == e_flag) { //If the 0th character is "M", the WIFI information is stored.
//    Serial.println("Stored information");
//    pinMode(SD_CS, OUTPUT);      digitalWrite(SD_CS, HIGH);
//    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
//    SPI.setFrequency(1000000);
//    SD.begin(SD_CS);
//    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
//    audio.setVolume(20); // 0...21
//    audio.connecttoFS(SD, "/123.mp3");
//    while (1)
//    {
//      unsigned long currentMillis = millis();
//      //Check to see if the time interval has been reached
//      if (currentMillis - previousBlinkTime >= blinkInterval) {    //If the time interval reaches
//        audio.stopSong();
//        break;
//      }
//      else if (currentMillis - previousBlinkTime <= 0)
//      { // If the millis time overflows
//        previousBlinkTime = currentMillis;
//      }
//      audio.loop();
//    }
//  }
//  else {
//    Serial.println("No WIFI information is stored, wifi information will be stored.");
//    //Save infomation
//    set_string(1, 0, e_flag, 0); //Entering the "N" character
//    Serial.println("Completion of storage");
//    Serial.println("It's about to be relaunched.");
//    ESP.restart();
//  }
//  EEPROM.end();

  //BLE
  BLEDevice::init(bleServerName);  //Create BLE and set the name
  pServer = BLEDevice::createServer();  //Creating a BLE server
  pServer->setCallbacks(new MyServerCallbacks());  //Setting up the connect and disconnect calling classes
  pService = pServer->createService(SERVICE_UUID); //Creating BLE service

  pCharacteristic = pService->createCharacteristic(  //Create ble feature（Characterristic_UUID）
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->setValue("ELECROW");
  //start broadcasting
  pAdvertising = BLEDevice::getAdvertising();  //A bleadvertising class pointer padvertising is defined, which points to bledevice:: getadvertising()
  pAdvertising->addServiceUUID(SERVICE_UUID);

  // Init Display
  lcd->begin();
  lcd->fillScreen(BLACK);
  lcd->setTextSize(2);
  delay(200);
#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  Serial.println("66666666666");
  ledcSetup(1, 300, 8);
  ledcAttachPin(TFT_BL, 1);
  ledcWrite(1, 255); /* Screen brightness can be modified by adjusting this parameter. (0-255) */

#endif
//  lcd->fillScreen(RED);
//  delay(1000);
//  lcd->fillScreen(GREEN);
//  delay(1000);
//  lcd->fillScreen(BLUE);
//  delay(1000);
//  lcd->fillScreen(BLACK);
//  delay(1000);
//  Serial.println("888");
  lv_init();

  // Init touch device
  Serial.println("999");
  touch_init();
  Serial.println("1111");

  screenWidth = lcd->width();
  screenHeight = lcd->height();
//#ifdef ESP32
//  disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * screenHeight / 10  , MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
//#else
//  disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * screenHeight / 10;
//#endif
//  if (!disp_draw_buf)
//  {
//    Serial.println("LVGL disp_draw_buf allocate failed!");
//  }
//  else
//  {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 15); //4

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();//Boot UI
    while (1)
    {
      Serial.print("goto_widget_flag:");
      Serial.print(goto_widget_flag);
      Serial.print("    ");
      Serial.print("bar_flag:");
      Serial.print(bar_flag);
      Serial.print("    ");
      Serial.print("first_flag:");
      Serial.println(first_flag);

      if (goto_widget_flag == 1)//Go to widget
      {
        if (ticker1.active() == true)
        {
          ticker1.detach();
        }
        goto_widget_flag = 0;
        delay(300);
        break;
      }

      if (goto_widget_flag == 3)//Go to the touch screen and close the progress bar thread first
      {
        bar_flag = 0; //Stop progress bar sign
        if (ticker1.active() == true)
        {
          ticker1.detach();
        }
        if (first_flag == 0 || first_flag == 1)
        {
          label_xy();
          first_flag = 2;
        }
        if (zero_clean == 1)
        {
          touch_last_x = 0;
          touch_last_y = 0;
          zero_clean = 0;
        }
        lv_label_set_text(ui_Label, "Touch Adjust:");
        lv_label_set_text_fmt(ui_Label3, "%d  %d", touch_last_x, touch_last_y); //Display touch information
      }

      if (goto_widget_flag == 4)//Touch the interface to return to the Menu interface, so that the progress bar zero restart
      {
        val = 100;
        delay(100);
        ticker1.attach_ms(35, callback1);//Callback1 is called every 20ms.
        goto_widget_flag = 0;
      }

      if (goto_widget_flag == 5) //Trigger calibration signal
      {
        lv_timer_handler();
        touch_calibrate();//Touch Calibration
        lv_scr_load_anim(ui_TOUCH, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
        lv_timer_handler();
        delay(100);
        goto_widget_flag = 3; //Access to the touch screen logo
        touch_last_x = 0;
        touch_last_y = 0;
      }

      if (bar_flag == 6)//Runs the progress bar once when you first boot into the Menu screen, then stops running after that
      {
        if (first_flag == 0)
        {
          lv_example_bar();
          ticker1.attach_ms(35, callback1);//Callback1 is called every 20ms.
          first_flag = 1;
        }
      }

      lv_timer_handler();
    }


    lcd->fillScreen(BLACK);
    lv_demo_widgets();//Main UI
    Serial.println( "Setup done" );
//  }
}

void loop()
{
  while (NO_Test_Flag == 0)
  {
    lv_timer_handler();
    CloseData = Serial.read();
    if (CloseData == 'b')  //Exit
    {
      NO_Test_Flag = 1;
      Test_Flag = 1;
      lcd->fillScreen(BLACK);
      Serial.println("Access to the test program");
      WiFi.disconnect();
      break;
    }
  }

  //*********************************Test Program***************************************
  //*********************************Test Program***************************************
  //*********************************Test Program***************************************
  while (Test_Flag == 1)
  {
    Ce_shi();
  }
}


//Touch Label Controls
void label_xy()
{
  ui_Label = lv_label_create(ui_TOUCH);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label, -55);
  lv_obj_set_y(ui_Label, -40);
  lv_obj_set_align(ui_Label, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

  ui_Label3 = lv_label_create(ui_TOUCH);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label3, 85);
  lv_obj_set_y(ui_Label3, -40);
  lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label3, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

}


//Progress bar control
void lv_example_bar(void)
{
  //////////////////////////////
  bar = lv_bar_create(ui_MENU);
  lv_bar_set_value(bar, 0, LV_ANIM_OFF);
  lv_obj_set_width(bar, 480);
  lv_obj_set_height(bar, 25);
  lv_obj_set_x(bar, 0);
  lv_obj_set_y(bar, 175);
  lv_obj_set_align(bar, LV_ALIGN_CENTER);
  lv_obj_set_style_bg_img_src(bar, &ui_img_bar_800_01_png, LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_set_style_bg_img_src(bar, &ui_img_bar_800_02_png, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_color(bar, lv_color_hex(0x2D8812), LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(bar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  //////////////////////
  ui_Labe2 = lv_label_create(bar);//Creating Tags
  lv_obj_set_style_text_color(ui_Labe2, lv_color_hex(0x09BEFB), LV_STATE_DEFAULT);
  lv_label_set_text(ui_Labe2, "0%");
  lv_obj_center(ui_Labe2);
}


void Ce_shi() {
  char serialData = Serial.read();
  switch (serialData) {
    case 'R':
      Close_Flag = 1;
      //      WiFi.disconnect();//Disconnect
      lcd->fillScreen(BLACK);
      Serial.println("Red Screen");
      while (Close_Flag == 1)
      {
        lcd->fillScreen(RED);
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd->fillScreen(BLACK);
          Serial.println("Exit Red Screen");
          break;
        }
      }
      break;

    case 'G':
      Close_Flag = 1;
      lcd->fillScreen(BLACK);
      Serial.println("green screen");
      while (Close_Flag == 1)
      {
        lcd->fillScreen(GREEN);
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd->fillScreen(BLACK);
          Serial.println("Exit green screen");
          break;
        }
      }
      break;

    case 'B':
      Close_Flag = 1;
      lcd->fillScreen(BLACK);
      Serial.println("Blue Screen");
      while (Close_Flag == 1)
      {
        lcd->fillScreen(BLUE);
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd->fillScreen(BLACK);
          Serial.println("Exit Blue Screen");
          break;
        }
      }
      break;

    case 'S':
      Close_Flag = 1;
      lcd->fillScreen(BLACK);
      Serial.println("TF Card initialization");
      SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
      SD_init();
      while (Close_Flag == 1)
      {
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd->fillScreen(BLACK);
          Serial.println("Exit TF Card initialization");
          break;
        }
      }
      break;

    case 'T':
      Close_Flag = 1;
      lcd->fillScreen(BLACK);
      Serial.println("touch screens");
      touch_flag = 0;
      delay(100);
      lcd->setCursor(300, 120);
      lcd->printf("Please Touch");
      Serial.println("TOUCH");
      touch_last_x=200;
      touch_last_y=200;
      delay(50);
      while (Close_Flag == 1)
      {
        if (touch_flag < 3)
        {
          if (touch_has_signal())
          {
            if (touch_touched())
            {
              if (touch_last_x > 0 && touch_last_x < 100 && touch_last_y > 0 && touch_last_y < 100 && touch_flag == 0)
              {
                lcd->fillScreen(BLACK);
                lcd->setCursor(320, 200);
                lcd->printf("               ");
                lcd->setCursor(320, 250);
                lcd->printf("               ");
  
                lcd->setCursor(320, 200);
                lcd->printf("x: %i     ", touch_last_x);
                lcd->setCursor(320, 250);
                lcd->printf("y: %i    ", touch_last_y);
                Serial.print( "Data x :" );
                Serial.println( touch_last_x );
                Serial.print( "Data y :" );
                Serial.println( touch_last_y );
                touch_flag = 1;
                
              }
              if (touch_last_x > 700 && touch_last_x < 800 && touch_last_y > 400 && touch_last_y < 480 && touch_flag == 1)
              {
                lcd->fillScreen(BLACK);
                lcd->setCursor(320, 200);
                lcd->printf("               ");
                lcd->setCursor(320, 250);
                lcd->printf("               ");
  
                lcd->setCursor(320, 200);
                lcd->printf("x: %i     ", touch_last_x);
                lcd->setCursor(320, 250);
                lcd->printf("y: %i    ", touch_last_y);
                Serial.print( "Data x :" );
                Serial.println( touch_last_x );
                Serial.print( "Data y :" );
                Serial.println( touch_last_y );
                touch_flag = 3;
                delay(100);
                Serial.println("OVER");
              }
              if (touch_last_x > 0 && touch_last_x < 4000 && touch_last_y > 0 && touch_last_y < 4000)
              {
                lcd->fillScreen(BLACK);
                lcd->setCursor(320, 200);
                lcd->printf("               ");
                lcd->setCursor(320, 250);
                lcd->printf("               ");
  
                lcd->setCursor(320, 200);
                lcd->printf("x: %i     ", touch_last_x);
                lcd->setCursor(320, 250);
                lcd->printf("y: %i    ", touch_last_y);
                Serial.print( "Data x :" );
                Serial.println( touch_last_x );
                Serial.print( "Data y :" );
                Serial.println( touch_last_y );
              }
              CloseData = Serial.read();
              if (CloseData == 'I')  //Exit
              {
                Close_Flag = 0;
                lcd->fillScreen(BLACK);
                break;
              }
              delay(80);
            }
          }
        }
        if (touch_flag == 3)
        {
          
          if (touch_touched())
          {
            lcd->fillScreen(BLACK);
            if (touch_last_x > 0 && touch_last_x < 4000 && touch_last_y > 0 && touch_last_y < 4000)
            {
                lcd->setCursor(320, 200);
                lcd->printf("               ");
                lcd->setCursor(320, 250);
                lcd->printf("               ");
  
                lcd->setCursor(320, 200);
                lcd->printf("x: %i     ", touch_last_x);
                lcd->setCursor(320, 250);
                lcd->printf("y: %i    ", touch_last_y);
                Serial.print( "Data x :" );
                Serial.println( touch_last_x );
                Serial.print( "Data y :" );
                Serial.println( touch_last_y );
            }
          }
        }
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd->fillScreen(BLACK);
          Serial.println("Exit touch screens");
          break;
        }
      }
      break;

    case 'L':
      Close_Flag = 1;
      Serial.println("IO Port Test");
      lcd->fillScreen(BLACK);
      pinMode(38, OUTPUT);
      digitalWrite(38, LOW);
      lcd->setCursor(260, 230);
      lcd->printf("IO port output testing");
      while (Close_Flag == 1)
      {
        digitalWrite(38, HIGH);//Pull high level to turn on the light
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          digitalWrite(38, LOW);
          lcd->fillScreen(BLACK);
          Serial.println("Exit IO Port Test");
          break;
        }
      }
      break;

    case 'i':
      Close_Flag = 1;
      u8g2.begin();
      delay(200);
      u8g2.enableUTF8Print();        // enable UTF8 support for the Arduino print() function
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.setFontDirection(0);
      Serial.println("IIC");
      lcd->fillScreen(BLACK);
      lcd->setCursor(280, 230);
      lcd->printf("IIC testing");
      while (Close_Flag == 1)
      {
        for (int i = 128; i > -90; i -= 20)
        {
          u8g2.firstPage();
          do {
            u8g2.drawStr(i, 25, "ELECROW");
            delay(2);
          } while ( u8g2.nextPage() );
        }
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd->fillScreen(BLACK);
          Serial.println("Exit IIC");
          break;
        }
      }
      break;

    case 'W':
      Close_Flag = 1;
      Serial.println("Wifi function testing");
      lcd->fillScreen(BLACK);
      lcd->setCursor(280, 230);
      lcd->printf("WiFi testing");
      wifi_flag = 0;
      while (Close_Flag == 1)
      {
        while (Serial.available()&&wifi_flag == 0)
        {
          CloseData = Serial.read();
          buf[bufindex++] = CloseData;
          if (CloseData == '\n')
          {
            buf[bufindex] = '\0';
            info[0] = buf;
            info[0] = strtok_r(info[0], "@", &info[1]);
            info[1] = strtok_r(info[1], "@", &info[2]);
            //sscanf(buf, "@%s@%s\n", wifi_account, wifi_password);
            delay(5);
            wifi_flag = 1;
          }
          if (bufindex >= 128)
          {
            bufindex = 0;
          }
        }
        if (wifi_flag == 1)
        {
          WiFi.disconnect();
          delay(100);
          WiFi.begin(info[0], info[1]);
          delay(100);
          Serial.println("begining");
          while (WiFi.status() != WL_CONNECTED)
          {
            delay(100);
            CloseData = Serial.read();
            if (CloseData == 'I')  //Exit
            {
              memset(buf, 0, 100);
              bufindex = 0;
              break;
            }
            Serial.println("connecting");
          }
          Serial.println("WiFi is connected.");
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          lcd->fillScreen(BLACK);
          lcd->setCursor(280, 230);
          lcd->printf("WiFi is connected");
          WiFi.disconnect();
          WiFi.mode(WIFI_OFF);
          wifi_flag = 2;
          memset(buf, 0, 100);
          bufindex = 0;
        }
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          wifi_flag = 0;
          Close_Flag = 0;
          memset(buf, 0, 100);
          bufindex = 0;
          WiFi.disconnect();
          lcd->fillScreen(BLACK);
          Serial.println("Exit Wifi function testing");
          break;
        }
      }
      break;


    case 'U':
      Close_Flag = 1;
      Serial.println("Open UART1");
      lcd->fillScreen(BLACK);
      lcd->setCursor(280, 230);
      lcd->printf("UART1 testing");
      while (Close_Flag == 1)
      {
        //  If the hardware serial port has data
        if (Serial.available())
        {
          //Read a byte from the hardware serial port and write it to the software serial port
          Serial.write(Serial.read());
        }
        //        //If the software serial port has data
        //        if (cardSerial.available())
        //        {
        //          //Read a byte from the software serial port and write it to the hardware serial port
        //          Serial.write(cardSerial.read());
        //        }
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          //          digitalWrite(25, LOW);
          lcd->fillScreen(BLACK);
          Serial.println("Close UART1");
          break;
        }
      }
      break;

//    case 'P':
//      Close_Flag = 1;
//      Serial.println("Open SPEAK");
//      lcd->fillScreen(BLACK);
//      lcd->setCursor(280, 230);
//      lcd->printf("SPEAK testing");
//      WiFi.disconnect();
//      delay(2000);
//      while (Close_Flag == 1)
//      {
//        //        audio.loop();
//        //        EEPROM.begin(1024);
//        //        set_string(1,0,"L",0); //Entering the "L" character
//        //        Serial.println("Completion of storage");
//        //        Serial.println("It's about to be relaunched.");
//        //        ESP.restart();
//        //        EEPROM.end();
//        CloseData = Serial.read();
//        if (CloseData == 'I')  //Exit
//        {
//          //          audio.stopSong();
//          //          pinMode(BUTTON_PIN, OUTPUT);
//          //          digitalWrite(BUTTON_PIN, LOW);
//          //          WiFi.disconnect();
//          delay(50);
//          Close_Flag = 0;
//          lcd->fillScreen(BLACK);
//          Serial.println("Close SPEAK");
//          break;
//        }
//      }
//      break;

    case 'V':
      Close_Flag = 1;
      Serial.println("Bluetooth function");
      pAdvertising->start();  //Start broadcasting
      pService->start();
      delay(500);
      lcd->fillScreen(BLACK);
      while (Close_Flag == 1)
      {
        if (connected_state == true) //equipped with a connection
        {
          lcd->fillScreen(BLACK);
          lcd->setCursor(260, 230);
          lcd->printf("Bluetooth is connected");
        }
        else
        {
          lcd->fillScreen(BLACK);
          lcd->setCursor(260, 230);
          lcd->printf("Bluetooth is not connected");
        }
        delay(100);
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          pAdvertising->stop();  //stop broadcasting
          pService->stop();
          delay(50);
          lcd->fillScreen(BLACK);
          Serial.println("Turn off Bluetooth function");
          break;
        }
      }
      break;

    //Exit Software Test Program
    case 'C':
      WiFi.disconnect();
      NO_Test_Flag = 0;
      Test_Flag = 0;
      Wire.end();
      lcd->fillScreen(BLACK);
      Serial.println("Exit the software test program");
      delay(500);
      break;
  }
}


//SD Cardinitialization
int SD_init()
{

  if (!SD.begin(SD_CS))
  {
    Serial.println("Card Mount Failed");
    return 1;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No TF card attached");
    return 1;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("TF Card Size: %lluMB\n", cardSize);
  listDir(SD, "/", 2);

  //  listDir(SD, "/", 0);
  //  createDir(SD, "/mydir");
  //  listDir(SD, "/", 0);
  //  removeDir(SD, "/mydir");
  //  listDir(SD, "/", 2);
  //  writeFile(SD, "/hello.txt", "Hello ");
  //  appendFile(SD, "/hello.txt", "World!\n");
  //  readFile(SD, "/hello.txt");
  //  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  //  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  //  Serial.println("SD init over.");

  return 0;
}

//Traversing the SD Card Catalog
void listDir(fs::FS & fs, const char *dirname, uint8_t levels)
{
  //  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    //Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  i = 0;
  while (file)
  {
    if (file.isDirectory())
    {
      //      Serial.print("  DIR : ");
      //      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("FILE: ");
      Serial.print(file.name());
      lcd->setCursor(0, 2 * i);
      lcd->printf("FILE:%s", file.name());

      Serial.print("SIZE: ");
      Serial.println(file.size());
      lcd->setCursor(180, 2 * i);
      lcd->printf("SIZE:%d", file.size());
      i += 16;
    }

    file = root.openNextFile();
  }
}

// optional
void audio_info(const char *info) {
  Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info) { //id3 metadata
  Serial.print("id3data     "); Serial.println(info);
}
void audio_eof_mp3(const char *info) { //end of file
  Serial.print("eof_mp3     "); Serial.println(info);
}
void audio_showstation(const char *info) {
  Serial.print("station     "); Serial.println(info);
}
void audio_showstreamtitle(const char *info) {
  Serial.print("streamtitle "); Serial.println(info);
}
void audio_bitrate(const char *info) {
  Serial.print("bitrate     "); Serial.println(info);
}
void audio_commercial(const char *info) { //duration in sec
  Serial.print("commercial  "); Serial.println(info);
}
void audio_icyurl(const char *info) { //homepage
  Serial.print("Please search again"); Serial.println(info);
}
void audio_lasthost(const char *info) { //stream URL played
  Serial.print("lasthost    "); Serial.println(info);
}



#define Z_THRESHOLD 350 // Touch pressure threshold for validating touches
#define _RAWERR 20 // Deadband error allowed in successive position samples
void begin_touch_read_write(void) {
  digitalWrite(38, HIGH); // Just in case it has been left low
  spi.setFrequency(600000);
  digitalWrite(38, LOW);
}

void end_touch_read_write(void) {
  digitalWrite(38, HIGH); // Just in case it has been left low
  spi.setFrequency(600000);

}

uint16_t getTouchRawZ(void) {

  begin_touch_read_write();

  // Z sample request
  int16_t tz = 0xFFF;
  spi.transfer(0xb0);               // Start new Z1 conversion
  tz += spi.transfer16(0xc0) >> 3;  // Read Z1 and start Z2 conversion
  tz -= spi.transfer16(0x00) >> 3;  // Read Z2

  end_touch_read_write();

  return (uint16_t)tz;
}

uint8_t getTouchRaw(uint16_t *x, uint16_t *y) {
  uint16_t tmp;

  begin_touch_read_write();

  // Start YP sample request for x position, read 4 times and keep last sample
  spi.transfer(0xd0);                    // Start new YP conversion
  spi.transfer(0);                       // Read first 8 bits
  spi.transfer(0xd0);                    // Read last 8 bits and start new YP conversion
  spi.transfer(0);                       // Read first 8 bits
  spi.transfer(0xd0);                    // Read last 8 bits and start new YP conversion
  spi.transfer(0);                       // Read first 8 bits
  spi.transfer(0xd0);                    // Read last 8 bits and start new YP conversion

  tmp = spi.transfer(0);                   // Read first 8 bits
  tmp = tmp << 5;
  tmp |= 0x1f & (spi.transfer(0x90) >> 3); // Read last 8 bits and start new XP conversion

  *x = tmp;

  // Start XP sample request for y position, read 4 times and keep last sample
  spi.transfer(0);                       // Read first 8 bits
  spi.transfer(0x90);                    // Read last 8 bits and start new XP conversion
  spi.transfer(0);                       // Read first 8 bits
  spi.transfer(0x90);                    // Read last 8 bits and start new XP conversion
  spi.transfer(0);                       // Read first 8 bits
  spi.transfer(0x90);                    // Read last 8 bits and start new XP conversion

  tmp = spi.transfer(0);                 // Read first 8 bits
  tmp = tmp << 5;
  tmp |= 0x1f & (spi.transfer(0) >> 3);  // Read last 8 bits

  *y = tmp;

  end_touch_read_write();

  return true;
}

uint8_t validTouch(uint16_t *x, uint16_t *y, uint16_t threshold) {
  uint16_t x_tmp, y_tmp, x_tmp2, y_tmp2;

  // Wait until pressure stops increasing to debounce pressure
  uint16_t z1 = 1;
  uint16_t z2 = 0;
  while (z1 > z2)
  {
    z2 = z1;
    z1 = getTouchRawZ();
    delay(1);
    Serial.print("z1:");
    Serial.println(z1);
  }


  if (z1 <= threshold) return false;

  getTouchRaw(&x_tmp, &y_tmp);


  delay(1); // Small delay to the next sample
  if (getTouchRawZ() <= threshold) return false;

  delay(2); // Small delay to the next sample
  getTouchRaw(&x_tmp2, &y_tmp2);


  if (abs(x_tmp - x_tmp2) > _RAWERR) return false;
  if (abs(y_tmp - y_tmp2) > _RAWERR) return false;

  *x = x_tmp;
  *y = y_tmp;

  return true;
}

void calibrateTouch(uint16_t *parameters, uint32_t color_fg, uint32_t color_bg, uint8_t size) {
  int16_t values[] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint16_t x_tmp, y_tmp;
  uint16_t _width = 800;
  uint16_t _height = 480;

  for (uint8_t i = 0; i < 4; i++) {
    lcd->fillRect(0, 0, size + 1, size + 1, color_bg);
    lcd->fillRect(0, _height - size - 1, size + 1, size + 1, color_bg);
    lcd->fillRect(_width - size - 1, 0, size + 1, size + 1, color_bg);
    lcd->fillRect(_width - size - 1, _height - size - 1, size + 1, size + 1, color_bg);

    if (i == 5) break; // used to clear the arrows

    switch (i) {
      case 0: // up left
        lcd->drawLine(0, 0, 0, size, color_fg);
        lcd->drawLine(0, 0, size, 0, color_fg);
        lcd->drawLine(0, 0, size , size, color_fg);
        break;
      case 1: // bot left
        lcd->drawLine(0, _height - size - 1, 0, _height - 1, color_fg);
        lcd->drawLine(0, _height - 1, size, _height - 1, color_fg);
        lcd->drawLine(size, _height - size - 1, 0, _height - 1 , color_fg);
        break;
      case 2: // up right
        lcd->drawLine(_width - size - 1, 0, _width - 1, 0, color_fg);
        lcd->drawLine(_width - size - 1, size, _width - 1, 0, color_fg);
        lcd->drawLine(_width - 1, size, _width - 1, 0, color_fg);
        break;
      case 3: // bot right
        lcd->drawLine(_width - size - 1, _height - size - 1, _width - 1, _height - 1, color_fg);
        lcd->drawLine(_width - 1, _height - 1 - size, _width - 1, _height - 1, color_fg);
        lcd->drawLine(_width - 1 - size, _height - 1, _width - 1, _height - 1, color_fg);
        break;
    }

    // user has to get the chance to release
    if (i > 0) delay(1000);

    for (uint8_t j = 0; j < 8; j++) {
      while (touch_has_signal())
      {
        if (touch_touched())
        {
          Serial.print( "Data x :" );
          Serial.println( touch_last_x );
          Serial.print( "Data y :" );
          Serial.println( touch_last_y );
          break; 
        }
      }
    }
  }
}


void touch_calibrate()//screen calibration
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  Serial.println("screen calibration");

  //calibration
  //  lcd->fillScreen(BLACK);
  //  lcd->setCursor(20, 0);
  //  Serial.println("setCursor");
  //  lcd->setTextFont(2);
  //  Serial.println("setTextFont");
  //  lcd->setTextSize(1);
  //  Serial.println("setTextSize");
  //  lcd->setTextColor(TFT_WHITE, TFT_BLACK);

  //  lcd->println("Touch corners as directed");
  Serial.println("Touch corners as directed");
  //  lcd->setTextFont(1);
  //  lcd->println();
  //      lcd->setCursor(175, 100);
  //      lcd->printf("Touch Adjust");
  //  Serial.println("setTextFont(1)");
  lv_timer_handler();
  calibrateTouch(calData, MAGENTA, BLACK, 17);
  Serial.println("calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15)");
  Serial.println(); Serial.println();
  Serial.println("// Use this calibration code in setup().");
  Serial.print("uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
  //  Serial.print("  tft.setTouch(calData);");
  //  Serial.println(); Serial.println();
  //  lcd->fillScreen(BLACK);
  //  lcd->println("XZ OK!");
  //  lcd->println("Calibration code sent to Serial port.");


}

void setTouch(uint16_t *parameters) {
  touchCalibration_x0 = parameters[0];
  touchCalibration_x1 = parameters[1];
  touchCalibration_y0 = parameters[2];
  touchCalibration_y1 = parameters[3];

  if (touchCalibration_x0 == 0) touchCalibration_x0 = 1;
  if (touchCalibration_x1 == 0) touchCalibration_x1 = 1;
  if (touchCalibration_y0 == 0) touchCalibration_y0 = 1;
  if (touchCalibration_y1 == 0) touchCalibration_y1 = 1;

  touchCalibration_rotate = parameters[4] & 0x01;
  touchCalibration_invert_x = parameters[4] & 0x02;
  touchCalibration_invert_y = parameters[4] & 0x04;
}
