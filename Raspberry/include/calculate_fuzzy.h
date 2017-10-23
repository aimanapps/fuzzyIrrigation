float f1;
float Er;
float dEr;
float pi = 3.141592654;

float fngrade(float X, float x1, float x2) ///fungsi trigonometri
{
	float fngrade = (X - x1) / (x2 - x1);
	return fngrade;
}

float theta(float f1, float Er, float dEr) ///fungsi menentukan kuadran 1,2,3,4
{
	float theta;
	if (dEr == 0) dEr = 0.00001;
    float sudut = atan(f1*dEr/Er) * 180/pi;
	if (Er > 0.00001 && dEr > 0) theta = sudut;
	else if (Er < 0.00001 && dEr > 0) theta = 180 - abs(sudut);
	else if (Er == 0.00001 && dEr > 0) theta = 180;
	else if (Er < 0.00001 && dEr < 0) theta = 180 + abs(sudut);
	else theta = 360 - abs(sudut);
	return theta;
}

float mN(float a) ///fungsi miuN
{
    float mN;
    if (a >= 0 && a <= 90) mN = 1;
    else if (a > 90 && a < 180) mN = fngrade(a, 180, 90);
    else if (a >= 180 && a <= 270) mN = 0;
    else mN = fngrade(a, 270, 360);
    return mN;
}

float Dk(float f1, float Er, float dEr) ///fungsi magnitude
{
    float Dk = sqrt(Er*Er + (f1*f1 * dEr*dEr));
    //printf("%f %f %f\n",f1,Er,dEr);
    return Dk;
}

float mD( float Dk, float f2) ///fungsi miuD
{
    float mD;
    if (Dk >= 0 && Dk <= f2) mD = fngrade(Dk, 0, f2);
    else mD = 1;
    return mD;
}

float Uk(float mN, float mD, float Um) ///fungsi Uk (hasil akhir)
{
    float Uk = ((1-(2*mN)*mD*Um));
    return Uk;
}

