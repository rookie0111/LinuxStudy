#include <func.h>
int sendfd(int sockfd, int fdtosend, int exitFlag){
    struct msghdr hdr;
    bzero(&hdr, sizeof(hdr));// name -> NULL namelen -> 0, 不能省略
    //正文部分必须要有，不管是否有用
    char buf[] = "hello"; // 正文数据
    struct iovec vec[1]; //数组记录离散区
    vec[0].iov_base = &exitFlag; // 第一个离散碎片首地址 
    vec[0].iov_len = sizeof(int);   // 第一个离散碎片的大小
    hdr.msg_iov = vec; // 将离散区信息存放到hdr中
    hdr.msg_iovlen = 1; // 离散区的个数
    // 控制字段，记录要发送的内核数据结构
    struct cmsghdr * pcmsg = (struct cmsghdr*)malloc(CMSG_LEN(sizeof(int)));
    pcmsg->cmsg_len = CMSG_LEN(sizeof(int)); // 对齐结构体
    pcmsg->cmsg_level = SOL_SOCKET; // 固定
    pcmsg->cmsg_type = SCM_RIGHTS;  // 文件描述符固定使用
    *(int*)CMSG_DATA(pcmsg) = fdtosend; // 通过pcmsg得到data首地址，强转int*，在进行解引用
    hdr.msg_control = pcmsg;  //将控制字段信息存入hdr中
    hdr.msg_controllen = CMSG_LEN(sizeof(int));
    int ret = sendmsg(sockfd, &hdr, 0);
    if (ret == -1){
        error(1, errno, "sendmsg");
    }
    return 0;
}
int recvfd(int sockfd, int *pfdtorecv, int *pexitFlag){
    struct msghdr hdr;
    bzero(&hdr, sizeof(hdr));
    char buf[6] = {0};
    struct iovec vec[1];
    vec[0].iov_base = pexitFlag;
    vec[0].iov_len = sizeof(int);
    hdr.msg_iov = vec;
    hdr.msg_iovlen = 1;

    struct cmsghdr * pcmsg = (struct cmsghdr*)malloc(CMSG_LEN(sizeof(int)));
    pcmsg->cmsg_len = CMSG_LEN(sizeof(int));
    pcmsg->cmsg_level = SOL_SOCKET;
    pcmsg->cmsg_type = SCM_RIGHTS;
    hdr.msg_control = pcmsg;
    hdr.msg_controllen = CMSG_LEN(sizeof(int));
    int ret = recvmsg(sockfd, &hdr, 0);
    if (ret == -1){
        error(1, errno, "recvmsg");
    }
    printf("buf = %s,  fd = %d\n", buf, *(int*)CMSG_DATA(pcmsg));
    *pfdtorecv = *(int*)CMSG_DATA(pcmsg);
    return 0;
}
