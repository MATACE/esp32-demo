#include <WebServer.h>
#include <DNSServer.h>

#include "wifi_web.h"

#define WEB_AP 1
#define WEB_STA 0

static const char *ssid = "ACE";          
static const char *password = "12345678";

static const char *username = "admin";     //用户名
static const char *userpassword = "12345678"; //用户密码

static String root_page = "<html><head><meta charset=\"UTF-8\"><title>ESP32 WebServer Test</title><script>function getData(){var t=window.XMLHttpRequest?new XMLHttpRequest:new ActiveXObject(\"Microsoft.XMLHTTP\");t.onreadystatechange=function(){4==t.readyState&&200==t.status&&(document.getElementById(\"txtRandomData\").innerHTML=t.responseText)},t.open(\"GET\",\"getRandomData\",!0),t.send()}</script></head><body><div id=\"txtRandomData\">Unkwon</div><input type=\"button\" value=\"random\" onclick=\"getData()\"></body></html>";

static IPAddress local_ip(192, 168, 4, 1); //IP地址
static IPAddress gateway(192, 168, 4, 1);  //网关地址
static IPAddress subnet(255, 255, 255, 0); //子网掩码
static const byte dns_port = 53; //DNS服务端口号，一般为53

static DNSServer dns_server; //声明DNSServer对象
static WebServer server(80);

int wifi_web_setup(void)
{
    unsigned int ret = 0;

    // 设置串口
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

#if WEB_STA
    // 设置wifi的模式
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); // 关闭wifi休眠
    WiFi.begin(ssid, password);
    
    while ((ret = WiFi.status()) != WL_CONNECTED) {
        delay(500);
        Serial.printf("WiFi connect failed errno %d\r\n", ret);
    }

    // 显示WiFi的ip地址
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
#endif

#if WEB_AP
    WiFi.mode(WIFI_AP); //设置为AP模式
    WiFi.setSleep(false);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid, password);

#endif

    // 绑定服务器链接
    server.on("/", [](){
        // 网页用户名和密码
        // 显示登录界面
        if (!server.authenticate(username, userpassword)) {
            // 登录失败, 显示登录界面
            return server.requestAuthentication();
        }
        // 显示登录界面
        server.send(200, "text/html", root_page);
    });
    //注册网页中ajax发送的get方法的请求和回调函数
    server.on("/getRandomData", HTTP_GET, [](){
        String message = "RandomNum: ";
        message += String(random(10000)); //取得随机数
        server.send(200, "text/plain", message); //将消息发送回页面
    }); 
    server.on("/p1", [](){ server.send(200, "text/html", "<!DOCTYPE html><html><head><meta charset=\"utf-8\"></head><body><h1>第一页面</h1></body></html>"); });
    server.on("/p2", [](){ server.send(200, "text/html", "<!DOCTYPE html><html><head><meta charset=\"utf-8\"></head><body><h1>第二页面</h1></body></html>"); });
    server.onNotFound([](){server.send(404, "text/html", "<!DOCTYPE html><html><head><meta charset=\"utf-8\"></head><body><h1>404问题</h1></body></html>"); }); 

#if WEB_AP
    dns_server.start(dns_port, "config.com", local_ip); //启动DNS服务,example.com即为注册的域名
#endif
    // 启动服务器
    server.begin();

    return 0;
}


int wifi_web_task(void)
{
#if WEB_AP
    dns_server.processNextRequest(); //处理来自客户端的请求
#endif
    // 处理客户端的请求
    server.handleClient();

    return 0;
}
