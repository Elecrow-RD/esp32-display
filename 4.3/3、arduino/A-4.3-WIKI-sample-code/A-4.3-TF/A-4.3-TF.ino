#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>

//4.3
#define SD_MOSI 11
#define SD_MISO 13
#define SD_SCK 12
#define SD_CS 10
void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 ); /*Serial Port Initializing */
  //SD Card
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
  delay(100);
  if (SD_init() == 1)
  {
    Serial.println("SD Cardinitialization failed！");
  }
  else
    Serial.println("SD Cardinitialization  successed");
}

void loop() {
  // put your main code here, to run repeatedly:

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
//  i = 0;
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
//      lcd.setCursor(0, 2 * i);
//      lcd.printf("FILE:%s", file.name());

      Serial.print("SIZE: ");
      Serial.println(file.size());
//      lcd.setCursor(180, 2 * i);
//      lcd.printf("SIZE:%d", file.size());
//      i += 16;
    }

    file = root.openNextFile();
  }
}
