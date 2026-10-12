#include <iostream>
#include <string>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

// BATASAN ERROR
const double ERROR_TOLERANCE = 0.0001;
const int MAX_ITERASI = 100;

// ============================================================
// PARSER & EVALUATOR PERSAMAAN MATEMATIKA
// ============================================================
class ExpressionEvaluator {
private:
    string expr;
    size_t pos;

    void skipWhitespace() {
        while (pos < expr.size() && isspace(expr[pos])) pos++;
    }

    char peek() {
        skipWhitespace();
        return (pos < expr.size()) ? expr[pos] : '\0';
    }

    char get() {
        skipWhitespace();
        return (pos < expr.size()) ? expr[pos++] : '\0';
    }

    string preprocess(const string& s) {
        string res = "";
        for (size_t i = 0; i < s.size(); i++) {
            res += s[i];
            if (i + 1 < s.size()) {
                char c1 = s[i], c2 = s[i + 1];
                if (isdigit(c1) && (c2 == 'x' || c2 == 'X' || c2 == '(' || isalpha(c2))) {
                    if (c1 != 'e' && c1 != 'E' && c2 != 'e' && c2 != 'E') res += '*';
                } else if ((c1 == 'x' || c1 == 'X' || c1 == ')') && 
                           (isdigit(c2) || c2 == '(' || c2 == 'x' || c2 == 'X' || isalpha(c2))) {
                    res += '*';
                }
            }
        }
        return res;
    }

    double parseExpression(double x) {
        double val = parseTerm(x);
        while (peek() == '+' || peek() == '-') {
            char op = get();
            val += (op == '+') ? parseTerm(x) : -parseTerm(x);
        }
        return val;
    }

    double parseTerm(double x) {
        double val = parsePower(x);
        while (peek() == '*' || peek() == '/') {
            char op = get();
            double div = parsePower(x);
            if (op == '*') val *= div;
            else val = (fabs(div) < 1e-15) ? 0 : val / div;
        }
        return val;
    }

    double parsePower(double x) {
        double val = parsePrimary(x);
        if (peek() == '^') {
            get();
            val = pow(val, parsePower(x));
        }
        return val;
    }

    double parsePrimary(double x) {
        skipWhitespace();
        char c = peek();

        if (c == '+') { get(); return parsePrimary(x); }
        if (c == '-') { get(); return -parsePrimary(x); }
        if (c == '(') {
            get();
            double val = parseExpression(x);
            if (peek() == ')') get();
            return val;
        }
        if (c == 'x' || c == 'X') { get(); return x; }

        if (isdigit(c) || c == '.') {
            size_t start = pos;
            while (pos < expr.size() && (isdigit(expr[pos]) || expr[pos] == '.')) pos++;
            return stod(expr.substr(start, pos - start));
        }

        if (isalpha(c)) {
            string name = "";
            while (pos < expr.size() && isalpha(expr[pos])) name += tolower(expr[pos++]);
            if (name == "e")  return exp(1.0);
            if (name == "pi") return acos(-1.0);

            double arg = parsePrimary(x);
            if (name == "sin") return sin(arg);
            if (name == "cos") return cos(arg);
            if (name == "tan") return tan(arg);
            if (name == "exp") return exp(arg);
            if (name == "ln" || name == "log") return log(arg);
            if (name == "sqrt") return sqrt(arg);
            if (name == "abs")  return fabs(arg);
        }
        return 0;
    }

public:
    ExpressionEvaluator() : expr("0"), pos(0) {}
    void setExpression(const string& e) { expr = preprocess(e); pos = 0; }
    double eval(double x) { pos = 0; return parseExpression(x); }
};

static ExpressionEvaluator evaluatorPersamaan;
static string persamaanUser = "";

// FUNGSI INPUT PERSAMAAN
void inputPersamaan() {
    cout << "Masukkan persamaan f(x):\n"
         << "Contoh format: x^3 - 6x^2 + 11x - 6 atau x^2 - 4\n"
         << "f(x) = ";
    if (cin.peek() == '\n') cin.ignore();
    getline(cin, persamaanUser);

    while (persamaanUser.empty()) {
        cout << "Persamaan tidak boleh kosong! f(x) = ";
        getline(cin, persamaanUser);
    }

    evaluatorPersamaan.setExpression(persamaanUser);
    cout << "\n-> Persamaan berhasil disimpan: f(x) = " << persamaanUser << "\n";
}

// EVALUASI f(x) DAN TURUNAN f'(x)
double f(double x) {
    return evaluatorPersamaan.eval(x);
}

double f_turunan(double x) {
    const double h = 1e-6;
    return (f(x + h) - f(x - h)) / (2.0 * h);
}

// FUNGSI INPUT ANGKA
double inputAngka(const string& pesan) {
    double nilai;
    cout << pesan;
    while (!(cin >> nilai)) {
        cout << "Input tidak valid. Masukkan angka: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return nilai;
}

// HELPER CETAK HASIL AKAR
void printHasilAkar(double c, int iterasi, double fc, int garis = 88) {
    cout << string(garis, '-') << "\n"
         << "Akar baru ditemukan : c = " << fixed << setprecision(6) << c << "\n"
         << "Jumlah iterasi      : " << iterasi << "\n"
         << "fc = f(c)           : " << fixed << setprecision(8) << fc << "\n"
         << "=======================================\n";
}

// HELPER VALIDASI INTERVAL METODE TERTUTUP
bool validasiInterval(double &a, double &b, double &fa, double &fb) {
    if (a > b) swap(a, b);
    fa = f(a);
    fb = f(b);

    cout << "Batas bawah (a) : " << a << "\n"
         << "Batas atas (b)  : " << b << "\n"
         << "Toleransi Error : " << ERROR_TOLERANCE << "\n"
         << "Max Iterasi     : " << MAX_ITERASI << "\n"
         << "---------------------------------------\n";

    if (fabs(fa) < ERROR_TOLERANCE) {
        cout << "Akar baru ditemukan di batas bawah: c = " << a << "\n"
             << "fc = f(c) = " << fa << "\n"
             << "=======================================\n";
        return false;
    }
    if (fabs(fb) < ERROR_TOLERANCE) {
        cout << "Akar baru ditemukan di batas atas: c = " << b << "\n"
             << "fc = f(c) = " << fb << "\n"
             << "=======================================\n";
        return false;
    }
    if (fa * fb > 0) {
        cout << "ERROR: fa dan fb harus berbeda tanda (fa * fb < 0)!\n"
             << "fa = f(" << a << ") = " << fa << "\n"
             << "fb = f(" << b << ") = " << fb << "\n"
             << "Tidak ada akar atau akar ganda pada interval ini.\n"
             << "=======================================\n";
        return false;
    }
    return true;
}

// METODE TABEL (METODE TERTUTUP)
void metodeTabel(double a, double b, double step) {
    cout << "\n=======================================\n"
         << "          METODE TABEL\n"
         << "=======================================\n"
         << "Persamaan       : f(x) = " << persamaanUser << "\n";

    if (a > b) swap(a, b);
    if (step <= 0) {
        cout << "ERROR: Step harus lebih besar dari 0!\n";
        return;
    }

    cout << "Batas bawah (a) : " << a << "\n"
         << "Batas atas (b)  : " << b << "\n"
         << "Step            : " << step << "\n"
         << "---------------------------------------\n"
         << setw(10) << "x" << setw(15) << "f(x)" << "\n"
         << "---------------------------------------\n";

    double c_terbaik = a, min_error = fabs(f(a));
    for (double x = a; x <= b; x += step) {
        double fx = f(x);
        cout << setw(10) << fixed << setprecision(4) << x
             << setw(15) << fixed << setprecision(6) << fx << "\n";
        if (fabs(fx) < min_error) {
            min_error = fabs(fx);
            c_terbaik = x;
        }
    }

    cout << "---------------------------------------\n"
         << "Perkiraan akar terbaik : c = " << fixed << setprecision(6) << c_terbaik << "\n"
         << "fc = f(c)              : " << fixed << setprecision(8) << f(c_terbaik) << "\n"
         << "=======================================\n";
}

// METODE BISEKSI (METODE TERTUTUP)
void metodeBiseksi(double a, double b) {
    cout << "\n=======================================\n"
         << "        METODE BISEKSI\n"
         << "=======================================\n"
         << "Persamaan       : f(x) = " << persamaanUser << "\n";

    double fa, fb;
    if (!validasiInterval(a, b, fa, fb)) return;

    cout << setw(8)  << "Iterasi"
         << setw(12) << "a"
         << setw(12) << "b"
         << setw(12) << "c"
         << setw(14) << "fa"
         << setw(14) << "fb"
         << setw(14) << "fc" << "\n"
         << string(88, '-') << "\n";

    double c = a, fc = 0;
    int iterasi = 0;

    while (iterasi < MAX_ITERASI) {
        fa = f(a);
        fb = f(b);
        c = (a + b) / 2.0;
        fc = f(c);
        iterasi++;

        cout << setw(8)  << iterasi
             << setw(12) << fixed << setprecision(6) << a
             << setw(12) << fixed << setprecision(6) << b
             << setw(12) << fixed << setprecision(6) << c
             << setw(14) << fixed << setprecision(6) << fa
             << setw(14) << fixed << setprecision(6) << fb
             << setw(14) << fixed << setprecision(6) << fc << "\n";

        if (fabs(fc) < ERROR_TOLERANCE) break;
        if (fa * fc < 0) b = c;
        else a = c;
    }

    printHasilAkar(c, iterasi, fc);
}

// METODE REGULA FALSI (METODE TERTUTUP)
void metodeRegulaFalsi(double a, double b) {
    cout << "\n=======================================\n"
         << "     METODE REGULA FALSI\n"
         << "=======================================\n"
         << "Persamaan       : f(x) = " << persamaanUser << "\n";

    double fa, fb;
    if (!validasiInterval(a, b, fa, fb)) return;

    cout << setw(8)  << "Iterasi"
         << setw(12) << "a"
         << setw(12) << "b"
         << setw(12) << "c"
         << setw(14) << "fa"
         << setw(14) << "fb"
         << setw(14) << "fc" << "\n"
         << string(88, '-') << "\n";

    double c = a, fc = 0;
    int iterasi = 0;

    while (iterasi < MAX_ITERASI) {
        fa = f(a);
        fb = f(b);

        if (fabs(fb - fa) < 1e-15) {
            cout << "ERROR: Pembagian dengan nol pada fb - fa!\n";
            break;
        }

        c = (a * fb - b * fa) / (fb - fa);
        fc = f(c);
        iterasi++;

        cout << setw(8)  << iterasi
             << setw(12) << fixed << setprecision(6) << a
             << setw(12) << fixed << setprecision(6) << b
             << setw(12) << fixed << setprecision(6) << c
             << setw(14) << fixed << setprecision(6) << fa
             << setw(14) << fixed << setprecision(6) << fb
             << setw(14) << fixed << setprecision(6) << fc << "\n";

        if (fabs(fc) < ERROR_TOLERANCE) break;
        if (fa * fc < 0) b = c;
        else a = c;
    }

    printHasilAkar(c, iterasi, fc);
}

// METODE NEWTON-RAPHSON (METODE TERBUKA)
void metodeNewtonRaphson(double a) {
    cout << "\n=======================================\n"
         << "     METODE NEWTON-RAPHSON\n"
         << "=======================================\n"
         << "Persamaan       : f(x) = " << persamaanUser << "\n"
         << "Tebakan awal (a): " << a << "\n"
         << "Toleransi Error : " << ERROR_TOLERANCE << "\n"
         << "Max Iterasi     : " << MAX_ITERASI << "\n"
         << "---------------------------------------\n"
         << setw(8)  << "Iterasi"
         << setw(12) << "a"
         << setw(12) << "c"
         << setw(14) << "fa"
         << setw(14) << "f'(a)"
         << setw(14) << "fc" << "\n"
         << string(86, '-') << "\n";

    int iterasi = 0;
    double c = a, fc = f(a);

    while (iterasi < MAX_ITERASI) {
        double fa = f(a);
        double dfa = f_turunan(a);

        if (fabs(dfa) < 1e-12) {
            cout << "\nERROR: f'(a) mendekati 0, metode tidak dapat dilanjutkan!\n"
                 << "Coba gunakan tebakan awal yang berbeda.\n";
            return;
        }

        c = a - fa / dfa;
        fc = f(c);
        iterasi++;

        cout << setw(8)  << iterasi
             << setw(12) << fixed << setprecision(6) << a
             << setw(12) << fixed << setprecision(6) << c
             << setw(14) << fixed << setprecision(6) << fa
             << setw(14) << fixed << setprecision(6) << dfa
             << setw(14) << fixed << setprecision(6) << fc << "\n";

        if (fabs(fc) < ERROR_TOLERANCE) break;
        a = c;
    }

    printHasilAkar(c, iterasi, fc, 86);
}

// FUNGSI UTAMA (MAIN)
int main() {
    int pilihan;
    double a, b, step;

    inputPersamaan();

    do {
        cout << "\n=======================================\n"
             << "Persamaan: f(x) = " << persamaanUser << "\n"
             << "1. Metode Tabel\n"
             << "2. Metode Biseksi\n"
             << "3. Metode Regula Falsi\n"
             << "4. Metode Newton-Raphson\n"
             << "5. Ganti Persamaan\n"
             << "0. Keluar\n"
             << "=======================================\n"
             << "Masukkan pilihan: ";

        if (!(cin >> pilihan)) {
            cout << "Input harus berupa angka!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1:
                a = inputAngka("Batas bawah (a): ");
                b = inputAngka("Batas atas (b) : ");
                do {
                    step = inputAngka("Step (h)       : ");
                    if (step <= 0) cout << "Step harus lebih besar dari 0!\n";
                } while (step <= 0);
                metodeTabel(a, b, step);
                break;

            case 2:
            case 3:
                a = inputAngka("Batas bawah (a): ");
                b = inputAngka("Batas atas (b) : ");
                if (pilihan == 2) metodeBiseksi(a, b);
                else metodeRegulaFalsi(a, b);
                break;

            case 4:
                a = inputAngka("Tebakan awal (a): ");
                metodeNewtonRaphson(a);
                break;

            case 5:
                inputPersamaan();
                break;

            case 0:
                cout << "Program selesai. Terima kasih!\n";
                break;

            default:
                cout << "Pilihan tidak valid!\n";
        }
    } while (pilihan != 0);

    return 0;
}
