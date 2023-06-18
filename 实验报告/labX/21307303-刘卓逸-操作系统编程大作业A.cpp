#include <iostream>
#include <stdio.h>
#include <pthread.h>

struct args{
    int (*a)[9]; // 数独
    bool *ans; // 要将结果返回的地方
    int rcb; // 当前任务是判断行还是列还是块
    int id; // 当前任务是判断第几个行列块
    args(int (*_a)[9]=nullptr,bool *_ans=nullptr,int _rcb=0,int _id=0):a(_a),ans(_ans),id(_id),rcb(_rcb){}
};
const char *(RCB[3])={"Row","Column","Block"};

void *check_rcb(void *_arg){
    // 参数传输
    args* arg=(args*)_arg;
    int (*a)[9]=arg->a;
    bool *ans=arg->ans;
    int rcb=arg->rcb;
    int i=arg->id;
    // 主模块
    bool hash[10]={0};
    for (int j=0;j<9;j++){
        int temp=(rcb==0)?a[i][j]:
                 (rcb==1)?a[j][i]:
                 a[(i/3)*3+j/3][(i%3)*3+j%3];
        if (temp<1||temp>9||hash[temp-1]){
            *ans=1;
            printf("---- checking %s %d Fail\n", RCB[rcb] , i);
            return nullptr;
        }
        else{
            hash[temp-1]=1;
        }
    }
    printf("---- checking %s %d Succeed\n",RCB[rcb],i);
    return nullptr;
}

void *check_rcbs(void *_arg){
    // 参数传输
    args* arg=(args*)_arg;
    int (*a)[9]=arg->a;
    bool *ans=arg->ans;
    int rcb=arg->rcb;
    // 参数准备
    bool ans2[9]={0};
    args arg2[9];
    for (int i=0;i<9;i++)
        arg2[i]=args(a,ans2+i,rcb,i);
    // 主模块
    pthread_t c_rcb[9];
    int iret_rcb[9];
    for (int i=0;i<9;i++)
        iret_rcb[i]=pthread_create(&c_rcb[i], NULL, check_rcb, (void*)(&arg2[i]));
    for (int i=0;i<9;i++)
        pthread_join(c_rcb[i], NULL);
    // 合并
    bool ansX=0;
    for (int i=0;i<9;i++)
        ansX|=ans2[i];
    if (ansX)
        printf("-- Checking %s Fail\n",RCB[rcb]);
    else
        printf("-- Checking %s Succeed\n",RCB[rcb]);
    *ans=ansX;
    return nullptr;
}

bool parallel_check(int a[][9]){
    // 参数准备
    bool ans[3]={0};
    args arg[3]={args(a,ans,0),args(a,ans+1,1),args(a,ans+2,2)};
    // 从创建线程，传递参数
    pthread_t c_cols, c_rows, c_blks;
    int iret_rows = pthread_create(&c_rows, NULL, check_rcbs, (void*)(&arg[0]));
    int iret_cols = pthread_create(&c_cols, NULL, check_rcbs, (void*)(&arg[1]));
    int iret_blks = pthread_create(&c_blks, NULL, check_rcbs, (void*)(&arg[2]));
    // 等待线程结束
    pthread_join(c_cols, NULL);
    pthread_join(c_rows, NULL);
    pthread_join(c_blks, NULL);
    // 整理结果
    bool ansX=!(ans[0]||ans[1]||ans[2]);
    printf("Checking %s\n",ansX?"Succeed":"Fail");
    return ansX;
}

int main()
{
    int a[9][9];
    for (int i=0;i<9;i++){
        for (int j=0;j<9;j++)
            scanf("%d",&a[i][j]);
    }
    if (parallel_check(a)){
        printf("Outside call: True");
    }
    else{
        printf("Outside call: False");
    }
    return 0;
}
