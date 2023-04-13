#include <WiFi.h>
#include <WebServer.h>
#include <Update.h> // 引用相关库

#include "esp_web_updata.h"

static const char *ssid = "********";
static const char *password = "********";

WebServer server(80);

// 以下为网页文件
String indexhtml = String("") +
                   "<!DOCTYPE html>\n" +
                   "<head>\n" +
                   "    <meta charset=\"UTF-8\">\n" +
                   "    <title>Update Test</title>\n" +
                   "</head>\n" +
                   "<body>\n" +
                   "    <form method=\'POST\' action=\'/update\' enctype=\'multipart/form-data\'>\n" +
                   "        <input type=\'file\' name=\'firmware\'>\n" +
                   "        <input type=\'submit\'>\n" +
                   "    </form>\n" +
                   "</body>\n";

bool shouldreboot = false; // 重启标志，固件升级操作完成后设置该标志准备重启设备

void handleResponse() // 回调函数
{
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
}

void handleFileupload() // 回调函数
{
    HTTPUpload &upload = server.upload();   // 文件上传对象
    if (upload.status == UPLOAD_FILE_START) // 文件上传开始
    {
        Serial.printf("开始上传文件: %s\n", upload.filename.c_str());
        if (!Update.begin()) // 开始固件升级，检查空可用间大小，如果正在升级或空间不够则返回false
        {
            Update.printError(Serial);
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE) // 文件读写中
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) // 将文件数据写入到OTA区域
        {
            Update.printError(Serial);
        }
    }
    else if (upload.status == UPLOAD_FILE_END) // 文件上传完成
    {
        Serial.println("写入" + String(upload.totalSize) + "字节到OTA区域");
        if (!Update.end(true)) // 完成数据写入，设置在系统重启后自动将OTA区域固件移动到Sketch区域 // Update.begin不指定大小时这里设置true
        {
            Update.printError(Serial);
        }
        Serial.println("升级操作完成，模块将在5秒后重启以完成固件升级");
        shouldreboot = true;
    }
    else
    {
        Serial.printf("固件上传失败: status=%d\n", upload.status);
    }
}

int esp_web_updata_setup(void)
{
        Serial.begin(115200);
    Serial.println();

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, []()
              {
                  server.sendHeader("Connection", "close");
                  server.send(200, "text/html", indexhtml); // 发送网页
              });

    server.on("/update", HTTP_POST, handleResponse, handleFileupload); // 绑定回调函数

    server.begin(); // 启动服务器

    Serial.println("Web server started");

    return 0;
}
int esp_web_updata_task(void)
{

    server.handleClient(); // 处理来自客户端的请求

    if (shouldreboot)
    {
        delay(5000);
        ESP.restart(); // 重启设备
    }
    return 0;
}

#if 0    // OTA下载

// https://github.com/espressif/arduino-esp32/tree/master/libraries/Update
// https://github.com/espressif/arduino-esp32/tree/master/libraries/ArduinoOTA
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/WebUpdate/WebUpdate.ino

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

const char* host = "esp32";
const char* ssid = "xxx";
const char* password = "xxxx";

WebServer server(80);

/*
 * Login page
 */

const char* loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
             "<td>Username:</td>"
             "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

/*
 * Server Index Page
 */

const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

/*
 * setup function
 */
void setup(void) {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void loop(void) {
  server.handleClient();
  delay(1);
}


#endif