#include "normalPID.h"
#include "assert.h"

NormalPID::NormalPID(double Kp, double Ti, double Td):
    m_dKp(Kp), m_dTi(Ti), m_dTd(Td)
{

}

NormalPID::~NormalPID()
{

}

double NormalPID::PIDCtrl(double error, double time)
{
	assert(time > 0);
	if (m_dTi == 0)
		return PDCtrl(error, time);

	if (m_ucMode == 1)//采集到第一个e(k)：位置式PI控U = kp*e+kp*T*e/ti
	{
		result = m_dKp*error + m_dKp*time*error / m_dTi;
		lastError = error;
		m_ucMode++;
	}
	else if (m_ucMode == 2)//采集到第二个e(k):位置式PID控U=kp*e1+ki*T*(e1+e0)+kd(e1-e0)/T
	{
		result = m_dKp*error + m_dKp*time*(error + lastError) / m_dTi + m_dKp*m_dTd*(error - lastError) / time;
		last2Error = lastError;
		lastError = error;
		m_ucMode++;
	}
	else if (m_ucMode >= 3)//采集到第三个及以上e(k)：增量式PID:U(k)=U(k-1)+Δu
		//Δu=kp(ek-ek1)+kp*T*ek/ti+kp*Td(ek-2ek1+ek2)/T
	{
		result = result + m_dKp*(error - lastError) + m_dKp*time*error / m_dTi + m_dKp*m_dTd*(error - 2 * lastError + last2Error) / time;
		last2Error = lastError;
		lastError = error;
	}
	else return 0;

	return result;
}

double NormalPID::PDCtrlGivenDiff(double error, double diff)
{
	//使用外部获得的微分项diff
	//位置式PD：U(k) = kp*(e(k)+td*diff)
	return m_dKp*(error + m_dTd*diff);
}

double NormalPID::PDCtrl(double error, double time)
{
	if (m_ucMode == 1)//采集到第一个e(k)：位置式P控U = kp*e
	{
		result = m_dKp*error;
		m_ucMode++;
	}
	else if (m_ucMode >= 2)//采集到第二个e(k):位置式PD控U=kp*e1+kd(e1-e0)/T
	{
		result = m_dKp*error + m_dKp*m_dTd*(error - lastError) / time;
	}

	last2Error = lastError;
	lastError = error;

	return result;
}
