#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

// DEFINISI FUNGSI f(x) DAN TURUNANNYA f'(x)
// Contoh: f(x) = x^3 - 6x^2 + 11x - 6
double f(double x) {
    return pow(x, 3) - 6 * pow(x, 2) + 11 * x - 6;
}

double f_turunan(double x) {
    return 3 * pow(x, 2) - 12 * x + 11;
}

// BATASAN ERROR
const double ERROR_TOLERANCE = 0.0001;
const int MAX_ITERASI = 100;

// METODE TABEL (METODE TERTUTUP)
// a = batas bawah, b = batas atas, c = akar baru
void metodeTabel(double a, double b, double step) {
    cout << "\n=======================================" << endl;
    cout << "        METODE TABEL" << endl;
    cout << "=======================================" << endl;
    
    if (a > b) swap(a, b);

    cout << "Batas bawah (a): " << a << endl;
    cout << "Batas atas (b) : " << b << endl;
    cout << "Step           : " << step << endl;
    cout << "---------------------------------------" << endl;
    cout << setw(10) << "x" 
         << setw(15) << "f(x)" << endl;
    cout << "---------------------------------------" << endl;

    double x = a;
    double prev_x = a;
    double prev_fx = f(a);
    double c_terbaik = a;
    double min_error = fabs(f(a));
    bool adaPerubahanTanda = false;
    double root_interval_kiri = a, root_interval_kanan = a;

    while (x <= b + (step / 2.0)) {
        double fx = f(x);
        cout << setw(10) << fixed << setprecision(4) << x 
             << setw(15) << fixed << setprecision(6) << fx << endl;

        // Cari x dengan |f(x)| terkecil
        if (fabs(fx) < min_error) {
            min_error = fabs(fx);
            c_terbaik = x;
        }

        // Cek perubahan tanda
        if (x != a && prev_fx * fx < 0) {
            adaPerubahanTanda = true;
            root_interval_kiri = prev_x;
            root_interval_kanan = x;
            // Interpolasi linier untuk perkiraan akar c
            double c_interpolasi = prev_x - prev_fx * (x - prev_x) / (fx - prev_fx);
            if (fabs(f(c_interpolasi)) < min_error) {
                min_error = fabs(f(c_interpolasi));
                c_terbaik = c_interpolasi;
            }
        }

        prev_x = x;
        prev_fx = fx;
        x += step;
    }

    cout << "---------------------------------------" << endl;
    if (adaPerubahanTanda) {
        cout << "Akar berada di antara interval: [" << root_interval_kiri << ", " << root_interval_kanan << "]" << endl;
    }
    cout << "Akar baru ditemukan : c = " << fixed << setprecision(6) << c_terbaik << endl;
    cout << "fc = f(c)           : " << fixed << setprecision(8) << f(c_terbaik) << endl;
    cout << "=======================================" << endl;
}

// METODE BISEKSI (METODE TERTUTUP)
// a = batas bawah, b = batas atas, c = akar baru
// fa = f(a), fb = f(b), fc = f(c)
void metodeBiseksi(double a, double b) {
    cout << "\n=======================================" << endl;
    cout << "        METODE BISEKSI" << endl;
    cout << "=======================================" << endl;

    if (a > b) swap(a, b);

    cout << "Batas bawah (a) : " << a << endl;
    cout << "Batas atas (b)  : " << b << endl;
    cout << "Toleransi Error : " << ERROR_TOLERANCE << endl;
    cout << "Max Iterasi     : " << MAX_ITERASI << endl;
    cout << "---------------------------------------" << endl;

    double fa = f(a);
    double fb = f(b);

    // Cek apakah batas bawah atau atas sudah merupakan akar
    if (fabs(fa) < ERROR_TOLERANCE) {
        cout << "Akar baru ditemukan di batas bawah: c = " << a << endl;
        cout << "fc = f(c) = " << fa << endl;
        cout << "=======================================" << endl;
        return;
    }
    if (fabs(fb) < ERROR_TOLERANCE) {
        cout << "Akar baru ditemukan di batas atas: c = " << b << endl;
        cout << "fc = f(c) = " << fb << endl;
        cout << "=======================================" << endl;
        return;
    }

    // Validasi: fa dan fb harus berbeda tanda (fa * fb < 0)
    if (fa * fb > 0) {
        cout << "ERROR: fa dan fb harus berbeda tanda (fa * fb < 0)!" << endl;
        cout << "fa = f(" << a << ") = " << fa << endl;
        cout << "fb = f(" << b << ") = " << fb << endl;
        cout << "Tidak ada akar atau akar ganda pada interval ini." << endl;
        cout << "=======================================" << endl;
        return;
    }

    cout << setw(8)  << "Iterasi"
         << setw(12) << "a"
         << setw(12) << "b"
         << setw(12) << "c"
         << setw(14) << "fa"
         << setw(14) << "fb"
         << setw(14) << "fc"
         << setw(12) << "Error" << endl;
    cout << string(88, '-') << endl;

    double c = a;
    double fc = 0;
    int iterasi = 0;

    while (iterasi < MAX_ITERASI) {
        fa = f(a);
        fb = f(b);
        c = (a + b) / 2.0;
        fc = f(c);
        double error = fabs(b - a) / 2.0;

        iterasi++;
        cout << setw(8)  << iterasi
             << setw(12) << fixed << setprecision(6) << a
             << setw(12) << fixed << setprecision(6) << b
             << setw(12) << fixed << setprecision(6) << c
             << setw(14) << fixed << setprecision(6) << fa
             << setw(14) << fixed << setprecision(6) << fb
             << setw(14) << fixed << setprecision(6) << fc
             << setw(12) << fixed << setprecision(6) << error << endl;

        if (fabs(fc) < ERROR_TOLERANCE || error < ERROR_TOLERANCE) {
            break;
        }

        // Pembaruan interval (a = batas bawah, b = batas atas)
        if (fa * fc < 0) {
            b = c;  // Akar berada di antara a dan c
        } else {
            a = c;  // Akar berada di antara c dan b
        }
    }

    cout << string(88, '-') << endl;
    cout << "Akar baru ditemukan : c = " << fixed << setprecision(6) << c << endl;
    cout << "Jumlah iterasi      : " << iterasi << endl;
    cout << "fc = f(c)           : " << fixed << setprecision(8) << fc << endl;
    cout << "=======================================" << endl;
}

// METODE REGULA FALSI (METODE TERTUTUP)
// a = batas bawah, b = batas atas, c = akar baru
// fa = f(a), fb = f(b), fc = f(c)
void metodeRegulaFalsi(double a, double b) {
    cout << "\n=======================================" << endl;
    cout << "     METODE REGULA FALSI" << endl;
    cout << "=======================================" << endl;

    if (a > b) swap(a, b);

    cout << "Batas bawah (a) : " << a << endl;
    cout << "Batas atas (b)  : " << b << endl;
    cout << "Toleransi Error : " << ERROR_TOLERANCE << endl;
    cout << "Max Iterasi     : " << MAX_ITERASI << endl;
    cout << "---------------------------------------" << endl;

    double fa = f(a);
    double fb = f(b);

    if (fabs(fa) < ERROR_TOLERANCE) {
        cout << "Akar baru ditemukan di batas bawah: c = " << a << endl;
        cout << "fc = f(c) = " << fa << endl;
        cout << "=======================================" << endl;
        return;
    }
    if (fabs(fb) < ERROR_TOLERANCE) {
        cout << "Akar baru ditemukan di batas atas: c = " << b << endl;
        cout << "fc = f(c) = " << fb << endl;
        cout << "=======================================" << endl;
        return;
    }

    if (fa * fb > 0) {
        cout << "ERROR: fa dan fb harus berbeda tanda (fa * fb < 0)!" << endl;
        cout << "fa = f(" << a << ") = " << fa << endl;
        cout << "fb = f(" << b << ") = " << fb << endl;
        cout << "Tidak ada akar atau akar ganda pada interval ini." << endl;
        cout << "=======================================" << endl;
        return;
    }

    cout << setw(8)  << "Iterasi"
         << setw(12) << "a"
         << setw(12) << "b"
         << setw(12) << "c"
         << setw(14) << "fa"
         << setw(14) << "fb"
         << setw(14) << "fc"
         << setw(12) << "Error" << endl;
    cout << string(88, '-') << endl;

    double c = a, c_lama = 0;
    double fc = 0;
    int iterasi = 0;

    while (iterasi < MAX_ITERASI) {
        fa = f(a);
        fb = f(b);

        if (fabs(fb - fa) < 1e-15) {
            cout << "ERROR: Pembagian dengan nol pada fb - fa!" << endl;
            break;
        }

        c = (a * fb - b * fa) / (fb - fa);
        fc = f(c);

        iterasi++;
        double error = (iterasi == 1) ? fabs(fc) : fabs(c - c_lama);

        cout << setw(8)  << iterasi
             << setw(12) << fixed << setprecision(6) << a
             << setw(12) << fixed << setprecision(6) << b
             << setw(12) << fixed << setprecision(6) << c
             << setw(14) << fixed << setprecision(6) << fa
             << setw(14) << fixed << setprecision(6) << fb
             << setw(14) << fixed << setprecision(6) << fc
             << setw(12) << fixed << setprecision(6) << error << endl;

        if (fabs(fc) < ERROR_TOLERANCE || (iterasi > 1 && error < ERROR_TOLERANCE)) {
            break;
        }

        // Pembaruan interval (a = batas bawah, b = batas atas)
        if (fa * fc < 0) {
            b = c;  // Akar berada di antara a dan c
        } else {
            a = c;  // Akar berada di antara c dan b
        }

        c_lama = c;
    }

    cout << string(88, '-') << endl;
    cout << "Akar baru ditemukan : c = " << fixed << setprecision(6) << c << endl;
    cout << "Jumlah iterasi      : " << iterasi << endl;
    cout << "fc = f(c)           : " << fixed << setprecision(8) << fc << endl;
    cout << "=======================================" << endl;
}

// METODE NEWTON-RAPHSON (METODE TERBUKA)
// a = tebakan awal / nilai saat ini
// c = akar yang baru
// fa = f(a), fc = f(c)
void metodeNewtonRaphson(double a) {
    cout << "\n=======================================" << endl;
    cout << "     METODE NEWTON-RAPHSON" << endl;
    cout << "=======================================" << endl;
    cout << "Tebakan awal (a): " << a << endl;
    cout << "Toleransi Error : " << ERROR_TOLERANCE << endl;
    cout << "Max Iterasi     : " << MAX_ITERASI << endl;
    cout << "---------------------------------------" << endl;

    cout << setw(8)  << "Iterasi"
         << setw(12) << "a"
         << setw(12) << "c"
         << setw(14) << "fa"
         << setw(14) << "f'(a)"
         << setw(14) << "fc"
         << setw(12) << "Error" << endl;
    cout << string(86, '-') << endl;

    int iterasi = 0;
    double c = a;
    double fc = f(a);

    while (iterasi < MAX_ITERASI) {
        double fa = f(a);
        double dfa = f_turunan(a);

        if (fabs(dfa) < 1e-12) {
            cout << "\nERROR: f'(a) mendekati 0, metode tidak dapat dilanjutkan!" << endl;
            cout << "Coba gunakan tebakan awal yang berbeda." << endl;
            return;
        }

        c = a - fa / dfa;  // c adalah akar baru
        fc = f(c);
        double error = fabs(c - a);

        iterasi++;
        cout << setw(8)  << iterasi
             << setw(12) << fixed << setprecision(6) << a
             << setw(12) << fixed << setprecision(6) << c
             << setw(14) << fixed << setprecision(6) << fa
             << setw(14) << fixed << setprecision(6) << dfa
             << setw(14) << fixed << setprecision(6) << fc
             << setw(12) << fixed << setprecision(6) << error << endl;

        if (fabs(fc) < ERROR_TOLERANCE || error < ERROR_TOLERANCE) {
            break;
        }

        a = c;
    }

    cout << string(86, '-') << endl;
    cout << "Akar baru ditemukan : c = " << fixed << setprecision(6) << c << endl;
    cout << "Jumlah iterasi      : " << iterasi << endl;
    cout << "fc = f(c)           : " << fixed << setprecision(8) << fc << endl;
    cout << "=======================================" << endl;
}

int main() {
    int pilihan;
    double a, b, step;

    cout << "=======================================" << endl;
    cout << "  PROGRAM PENCARIAN AKAR PERSAMAAN" << endl;
    cout << "  f(x) = x^3 - 6x^2 + 11x - 6" << endl;
    cout << "=======================================" << endl;

    do {
        cout << "\nPilih metode yang ingin digunakan:" << endl;
        cout << "1. Metode Tabel        (Metode Tertutup)" << endl;
        cout << "2. Metode Biseksi      (Metode Tertutup)" << endl;
        cout << "3. Metode Regula Falsi (Metode Tertutup)" << endl;
        cout << "4. Metode Newton-Raphson (Metode Terbuka)" << endl;
        cout << "0. Keluar" << endl;
        cout << "---------------------------------------" << endl;
        cout << "Masukkan pilihan (0-4): ";

        if (!(cin >> pilihan)) {
            cout << "\nInput tidak valid! Harap masukkan angka." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1:
                cout << "\n--- Metode Tabel ---" << endl;
                cout << "Masukkan batas bawah (a): ";
                cin >> a;
                cout << "Masukkan batas atas (b) : ";
                cin >> b;
                cout << "Masukkan step           : ";
                cin >> step;
                metodeTabel(a, b, step);
                break;

            case 2:
                cout << "\n--- Metode Biseksi ---" << endl;
                cout << "Masukkan batas bawah (a): ";
                cin >> a;
                cout << "Masukkan batas atas (b) : ";
                cin >> b;
                metodeBiseksi(a, b);
                break;

            case 3:
                cout << "\n--- Metode Regula Falsi ---" << endl;
                cout << "Masukkan batas bawah (a): ";
                cin >> a;
                cout << "Masukkan batas atas (b) : ";
                cin >> b;
                metodeRegulaFalsi(a, b);
                break;

            case 4:
                cout << "\n--- Metode Newton-Raphson ---" << endl;
                cout << "Masukkan tebakan awal (a): ";
                cin >> a;
                metodeNewtonRaphson(a);
                break;

            case 0:
                cout << "\nProgram selesai. Terima kasih!" << endl;
                break;

            default:
                cout << "\nPilihan tidak valid!" << endl;
                break;
        }

    } while (pilihan != 0);

    return 0;
}
