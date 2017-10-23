#include "include/call_header.h"
#define ADDRESS 0x04

static const char *devName = "/dev/i2c-1";
static const char *URL_CURL_NEW= "curl -X POST -H \"Content-Type: application/x-www-form-urlencoded\" -H \"Cache-Control: no-cache\" -H \"Postman-Token: d0e062b4-49ee-6255-5e05-324b1bad112f\" -d \'hpsp=%f&hpc=%f&uk=%f&opt=%f\' \"https://dweet.io:443/dweet/for/sigap?key=6CcyGc3rKkTnSXi0sbnhmB\"";

void sendDataToServer(double hpsp, double hpc, double uk, double opt)
{
    char str[500];
    sprintf(str, URL_CURL_NEW, hpsp, hpc, uk, opt);
    puts(str);
    system(str);
    printf("\n");
}

int main()
{

    printf("I2C: Connecting\n");
    int file;
    float awal = 0.0;

    if ((file = open(devName, O_RDWR)) < 0)
    {
        fprintf(stderr, "I2C: Failed to access %d\n", devName);
        exit(1);
    }
    printf("I2C: acquiring buss to 0x%x\n", ADDRESS);

    if (ioctl(file, I2C_SLAVE, ADDRESS) < 0)
    {
        fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
        exit(1);
    }

    while(true)
    {
        long val, sleepp;
        double OpTime;
        unsigned char cmd[16];

        cmd[0] = val;
        usleep(10000);
        char buf[1];

        if (read(file, buf, 1) == 1)
        {
            int temp = (double) buf[0];
            temp = 15-temp; //kalibrasi ketinggian tergantung wadah
            printf("Received %d\n", temp);

            int n = 2;
            float Er[n], dEr[n];
            float HPSp = 7.0;
            float f1 = 0.83;
            float f2 = 1.00;
            float Um = 1.00;
            float __Uk[n];
            float HPc[n] = {awal, temp};

            double dur1, dur2, dur3;

            for (int i=0; i<n; i++)  //perhitungan error
            {
                Er[i] = HPc[i]-HPSp;
                printf("Er %f\n", Er[i]);
            }
            for (int i=0; i<n; i++)  //perhitungan delta error
            {
                if (i==0) dEr[i] = Er[i];
                else if (i==10) dEr[10] = 0;
                else dEr[i] = Er[i]-Er[i-1];
                printf("dEr %f\n", dEr[i]);
            }
            for (int i = 0; i < n; i++)  //variable yang diperlukan untuk mendapatkan Uk
            {
                float _theta = theta(f1, Er[i], dEr[i]);
                float _Dk = Dk(f1, Er[i], dEr[i]);
                float _mN = mN(_theta);
                float _mD = mD(_Dk, f2);
                float _Uk = Uk(_mN, _mD, Um);

                __Uk[i] = _Uk;
            }
            for (int i = 0; i < n; i++)
            {
                printf("Uk %f\n", __Uk[i]);
            }

            if(__Uk[1]>0)
            {
                double time = __Uk[1];
                //double dur = time*4*60;
                double dur = time*70; //waktu yang diperlukan untuk mengisi air dari ketinggian -10cm hingga 0cm
                if(temp >= 0)
                {
                    dur = 4*dur;
                } //aproksimasi waktu yang diperlukan ketika air berada di atas permukaan media (tanah/pasir)
                OpTime = dur;
                sleepp = (int)dur + 10; //durasi untuk sleep
                printf("Durasi %f\n", dur);
                //implementasi durasi agar dapat diterapkan di arduino
                dur1 = dur/255;
                dur1 = (int)dur1;
                dur2 = dur - (dur1*255);
                printf("Konstanta %f\n", dur1);
                printf("Tambahan  %f\n", dur2);
                int dur4 = dur1;
                int dur5 = dur2;

                cmd[0] = dur4; //pengiriman waktu ke arduino
                write(file, cmd, 1);
                cmd[0] = dur5;
                write(file, cmd, 1);

                awal = temp;
                sendDataToServer(HPSp, HPc[1], __Uk[1], OpTime);
                sleep(sleepp); //sleep digunakan untuk kompensasi waktu penyerapan air ke dalam toples yang disediakan
            }

            else //drainase
            {
                //double time = __Uk[1];
                //double dur = time*70; // dua baris ini digunakan jika waktu drainase telah diketahui
                OpTime = 0;
                awal = temp;
                sendDataToServer(HPSp, HPc[1], __Uk[1], OpTime);
                sleep(10); //sleep digunakan untuk kompensasi waktu penyerapan air ke dalam toples yang disediakan dan kompensasi pengiriman data jika ada kendala
            }
        }
    }

    close(file);
    return (EXIT_SUCCESS);
}
