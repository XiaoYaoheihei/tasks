#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdlib.h>
#define BUFSIZE 10000

int chdir(const char *);
void printpathcur(ino_t);
ino_t get_node (char *);
void print_node_name (ino_t , char *, int); 

//因为目录是特殊的文件，所以目录名就是文件名
int main() {
    printpathcur(get_node("."));        //通过当前的i-node找到当前的文件并打印路径
    putchar('\n');
    return 0;
}

void printpathcur(ino_t cur_inode) {
    ino_t befor_inode;                  //上一个目录的inode
    char cur_name[BUFSIZE];             //当前目录的名字
    //把文件的inode传进来
    if (get_node("..") != cur_inode) {  //根目录的当前目录和父目录inode一样
        chdir("..");                    //找爸爸，更改当前的工作目录
        print_node_name(cur_inode, cur_name, BUFSIZE);
        befor_inode = get_node(".");    //当前的目录已经更改
        printpathcur(befor_inode);
        printf("/%s", cur_name);      
    } else {                            //自行打印根目录就好了
        printf("/home");
    }

}

void print_node_name (ino_t cur_inode, char *name, int buflen) {
    //因为在调用这个函数之前已经将当前目录更改了
    //所以open的是上一个目录，是在上一个目录里面寻找子目录的inode
    DIR *dirp = opendir(".");//目录流
    struct dirent *direntemp;
    if (dirp == NULL) {
        perror(".");
        exit(1);
    }
    while((direntemp = readdir(dirp)) != NULL) {        //打开每一个目录流
        if (direntemp->d_ino == cur_inode) {            //判断流里面的inode是不是子的
            strncpy(name, direntemp->d_name, buflen);   //只是对字符串的一个复制过程
            name[buflen-1] = '\0';//以防万一
            closedir(dirp);
            return;
        }
    }
    fprintf(stderr, "error looking for %ld", cur_inode);
    exit(1);
}

ino_t get_node (char *fname) {
    //通过文件名找到所属目录的inode
    struct stat info;               //获取文件的所有相关信息
    if (stat(fname, &info) == -1) {
        fprintf(stderr, "Cannot open this file");
        perror(fname);
        exit(1);
    }
    return info.st_ino;             //成功获取当前目录文件的inode
}