# 以下程序由deepseek按照描述生成，我仅在第71行加了一个右括号

# 对deepseek的提示：
# 请编写一个在Linux下的C语言程序。
# 用于对某一给定服务器提供的webserver服务进行并行测试，
# 输入参数为被测服务器地址、测试次数。
# 如果服务链接建立且正常相应显示测试正常和测试次数参数；
# 如果服务链接未建立打印失败原因和测试次数

# 第一次生成的程序采用了多线程模式，
# 原因，对其中测试次数描述不清楚，deepseek误解为每次程序运行测试多少次

# 对deepseek给出的修改提示:
# 对上述程序进行修改：1、参数中所给的服务器地址直接是IP地址；
# 2、每次调用程序只运行一次请求，不用采用多线程方式；
# 3、上面所提到的测试次数是脚本传入的参数，代表了程序运行的第几次运行；
# 4、其中IP地址可以输入，默认值为192.168.0.101；
# 5、第几次运行也为可选参数，不输入时显示第几次运行出错时显示-1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 80
#define BUFFER_SIZE 4096
#define DEFAULT_IP "192.168.0.101"

int main(int argc, char *argv[]) {
    char *server_ip = DEFAULT_IP;   // 默认服务器IP地址
    int test_count = -1;            // 默认值-1表示未指定测试次数
    
    // 解析命令行参数
    if (argc >= 2) {
        server_ip = argv[1];
    }
    if (argc >= 3) {
        test_count = atoi(argv[2]);
    }
    
    int sockfd;
    struct sockaddr_in server_addr;
    char request[256];
    char response[BUFFER_SIZE];
    
    // 创建socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("测试 %d: 创建socket失败 - %s\n", test_count, strerror(errno));
        exit(1);
    }
    
    // 设置服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // 转换IP地址
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        printf("测试 %d: 无效的IP地址: %s\n", test_count, server_ip);
        close(sockfd);
        exit(1);
    }
    
    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        printf("测试 %d: 连接服务器失败 - %s\n", test_count, strerror(errno));
        close(sockfd);
        exit(1);
    }
    
    // 发送HTTP请求
    snprintf(request, sizeof(request), "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", server_ip);
    if (write(sockfd, request, strlen(request)) < 0) {
        printf("测试 %d: 发送请求失败 - %s\n", test_count, strerror(errno));
        close(sockfd);
        exit(1);
    }
    
    // 读取响应
    memset(response, 0, BUFFER_SIZE);
    if (read(sockfd, response, BUFFER_SIZE - 1) < 0) {
        printf("测试 %d: 读取响应失败 - %s\n", test_count, strerror(errno));
        close(sockfd);
        exit(1);
    }
    
    // 检查响应是否包含HTTP响应码
    if (strstr(response, "HTTP/1.") != NULL) {
        printf("测试 %d: 连接正常 - 服务器响应正常\n", test_count);
    } else {
        printf("测试 %d: 连接异常 - 无效的HTTP响应\n", test_count);
    }
    
    close(sockfd);
    return 0;
}
