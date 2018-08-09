#ifndef FUNCS_H
#define FUNCS_H

//求数组的中值
//p:数组头指针，a:数组个数
double median(double *p, int a);


/*带符号的角度差值计算函数
* 参数：参数2为基准，向右为正，向左为负
* 返回：带符号的角度差值*/
double signedDeltaAngle(double tgt, double base);


#endif // FUNCS_H
