#ifndef NORMALPID_H
#define NORMALPID_H

//作者：李逸琛

class NormalPID
{
public:
	NormalPID(double Kp, double Ti, double Td);
	~NormalPID();

	void reset(){ m_ucMode = 1; }//重置积分
	void setPara(double Kp, double Ti, double Td){ kp = Kp; ti = Ti; td = Td; }//设定参数

	double PIDCtrlGivenDiff(double error, double diff, double time);
	double PIDCtrl(double error, double time);//输出PID控制结果。如果td为0，则会调用PDCtrl
	double PDCtrlGivenDiff(double error, double diff);
	double PDCtrl(double error, double time);

	double kp;//比例系数Kp（越大，快速性越好，精度越高，超调增加，稳定性变差）
	double ti;//积分时间Ti（越小，稳态误差消除的越快，破坏稳定性）
	double td;//微分时间Td（越大，越稳定，增加调节时间，降低抗干扰性能）

protected:
	unsigned char m_ucMode = 1;
	double result = 0;
	double lastError = 0;
	double last2Error = 0;
};

#endif // NORMALPID_H
