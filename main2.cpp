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
                char c1 = s[i];
                char c2 = s[i + 1];
                if (isdigit(c1) && (c2 == 'x' || c2 == 'X' || c2 == '(' || isalpha(c2))) {
                    if ((c1 != 'e' && c1 != 'E') && !(c2 == 'e' || c2 == 'E')) {
                        res += '*';
                    }
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
        while (true) {
            char op = peek();
            if (op == '+') {
                get();
                val += parseTerm(x);
            } else if (op == '-') {
                get();
                val -= parseTerm(x);
            } else {
                break;
            }
        }
        return val;
    }

    double parseTerm(double x) {
        double val = parsePower(x);
        while (true) {
            char op = peek();
            if (op == '*') {
                get();
                val *= parsePower(x);
            } else if (op == '/') {
                get();
                double div = parsePower(x);
                if (fabs(div) < 1e-15) val = 0;
                else val /= div;
            } else {
                break;
            }
        }
        return val;
    }

    double parsePower(double x) {
        double val = parsePrimary(x);
        if (peek() == '^') {
            get();
            double p = parsePower(x);
            val = pow(val, p);
        }
        return val;
    }

    double parsePrimary(double x) {
        skipWhitespace();
        char c = peek();

        if (c == '+') {
            get();
            return parsePrimary(x);
        }
        if (c == '-') {
            get();
            return -parsePrimary(x);
        }

        if (c == '(') {
            get();
            double val = parseExpression(x);
            if (peek() == ')') get();
            return val;
        }

        if (c == 'x' || c == 'X') {
            get();
            return x;
        }

        if (isdigit(c) || c == '.') {
            size_t start = pos;
            while (pos < expr.size() && (isdigit(expr[pos]) || expr[pos] == '.')) {
                pos++;
            }
            return stod(expr.substr(start, pos - start));
        }

        if (isalpha(c)) {
            string name = "";
            while (pos < expr.size() && isalpha(expr[pos])) {
                name += tolower(expr[pos++]);
            }
            if (name == "e") return exp(1.0);
            if (name == "pi") return acos(-1.0);

            double arg = parsePrimary(x);
            if (name == "sin") return sin(arg);
            if (name == "cos") return cos(arg);
            if (name == "tan") return tan(arg);
            if (name == "exp") return exp(arg);
            if (name == "ln" || name == "log") return log(arg);
            if (name == "sqrt") return sqrt(arg);
            if (name == "abs") return fabs(arg);
        }

        return 0;
    }

public:
    ExpressionEvaluator() : expr("0"), pos(0) {}
    ExpressionEvaluator(const string& e) { setExpression(e); }

    void setExpression(const string& e) {
        expr = preprocess(e);
        pos = 0;
    }

    double eval(double x) {
        pos = 0;
        return parseExpression(x);
    }
};

static ExpressionEvaluator evaluatorPersamaan;
static string persamaanUser = "";

// FUNGSI UNTUK INPUT PERSAMAAN
void inputPersamaan() {
    cout << "Masukkan persamaan f(x):" << endl;
    cout << "Contoh format: x^3 - 6x^2 + 11x - 6 atau x^2 - 4" << endl;
    cout << "f(x) = ";
    
    if (cin.peek() == '\n') {
        cin.ignore();
    }
    getline(cin, persamaanUser);

    while (persamaanUser.empty()) {
        cout << "Persamaan tidak boleh kosong! f(x) = ";
        getline(cin, persamaanUser);
    }

    evaluatorPersamaan.setExpression(persamaanUser);
    cout << "\n-> Persamaan berhasil disimpan: f(x) = " << persamaanUser << endl;
}

// EVALUASI f(x) DAN TURUNAN f'(x)
double f(double x) {
    return evaluatorPersamaan.eval(x);
}

double f_turunan(double x) {
    const double h = 1e-6;
    return (f(x + h) - f(x - h)) / (2.0 * h);
}

// METODE TABEL (METODE TERTUTUP)
// a = batas bawah, b = batas atas, c = akar baru
void metodeTabel(double a, double b, double step) {
    cout << "\n=======================================" << endl;
    cout << "          METODE TABEL" << endl;
    cout << "=======================================" << endl;
    cout << "Persamaan       : f(x) = " << persamaanUser << endl;

    if (a > b) swap(a, b);

    if (step <= 0) {
        cout << "ERROR: Step harus lebih besar dari 0!" << endl;
        return;
    }

    cout << "Batas bawah (a) : " << a << endl;
    cout << "Batas atas (b)  : " << b << endl;
    cout << "Step            : " << step << endl;
    cout << "---------------------------------------" << endl;

    cout << setw(10) << "x"
         << setw(15) << "f(x)" << endl;
    cout << "---------------------------------------" << endl;

    double c_terbaik = a;
    double min_error = fabs(f(a));

    for (double x = a; x <= b; x += step) {
        double fx = f(x);

        cout << setw(10) << fixed << setprecision(4) << x
             << setw(15) << fixed << setprecision(6) << fx
             << endl;

        if (fabs(fx) < min_error) {
            min_error = fabs(fx);
            c_terbaik = x;
        }
    }

    cout << "---------------------------------------" << endl;
    cout << "Perkiraan akar terbaik : c = "
         << fixed << setprecision(6) << c_terbaik << endl;
    cout << "fc = f(c)              : "
         << fixed << setprecision(8) << f(c_terbaik) << endl;
    cout << "=======================================" << endl;
}

// METODE BISEKSI (METODE TERTUTUP)
// a = batas bawah, b = batas atas, c = akar baru
void metodeBiseksi(double a, double b) {
    cout << "\n=======================================" << endl;
    cout << "        METODE BISEKSI" << endl;
    cout << "=======================================" << endl;
    cout << "Persamaan       : f(x) = " << persamaanUser << endl;

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
         << setw(14) << "fc" << endl;
    cout << string(88, '-') << endl;

    double c = a;
    double fc = 0;
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
             << setw(14) << fixed << setprecision(6) << fc << endl;

        if (fabs(fc) < ERROR_TOLERANCE) {
            break;
        }

        if (fa * fc < 0) {
            b = c;
        } else {
            a = c;
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
void metodeRegulaFalsi(double a, double b) {
    cout << "\n=======================================" << endl;
    cout << "     METODE REGULA FALSI" << endl;
    cout << "=======================================" << endl;
    cout << "Persamaan       : f(x) = " << persamaanUser << endl;

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
         << setw(14) << "fc" << endl;
    cout << string(88, '-') << endl;

    double c = a;
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

        cout << setw(8)  << iterasi
             << setw(12) << fixed << setprecision(6) << a
             << setw(12) << fixed << setprecision(6) << b
             << setw(12) << fixed << setprecision(6) << c
             << setw(14) << fixed << setprecision(6) << fa
             << setw(14) << fixed << setprecision(6) << fb
             << setw(14) << fixed << setprecision(6) << fc << endl;

        if (fabs(fc) < ERROR_TOLERANCE) {
            break;
        }

        if (fa * fc < 0) {
            b = c;
        } else {
            a = c;
        }
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
void metodeNewtonRaphson(double a) {
    cout << "\n=======================================" << endl;
    cout << "     METODE NEWTON-RAPHSON" << endl;
    cout << "=======================================" << endl;
    cout << "Persamaan       : f(x) = " << persamaanUser << endl;
    cout << "Tebakan awal (a): " << a << endl;
    cout << "Toleransi Error : " << ERROR_TOLERANCE << endl;
    cout << "Max Iterasi     : " << MAX_ITERASI << endl;
    cout << "---------------------------------------" << endl;

    cout << setw(8)  << "Iterasi"
         << setw(12) << "a"
         << setw(12) << "c"
         << setw(14) << "fa"
         << setw(14) << "f'(a)"
         << setw(14) << "fc" << endl;
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

        iterasi++;
        cout << setw(8)  << iterasi
             << setw(12) << fixed << setprecision(6) << a
             << setw(12) << fixed << setprecision(6) << c
             << setw(14) << fixed << setprecision(6) << fa
             << setw(14) << fixed << setprecision(6) << dfa
             << setw(14) << fixed << setprecision(6) << fc << endl;

        if (fabs(fc) < ERROR_TOLERANCE) {
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

double inputAngka(string pesan) {
    double nilai;
    cout << pesan;

    while (!(cin >> nilai)) {
        cout << "Input tidak valid. Masukkan angka: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    return nilai;
}


int main() {
    int pilihan;
    double a, b, step;

    inputPersamaan();

    do {
        cout << "\n=======================================\n"
             << "Persamaan: f(x) = " << persamaanUser << '\n'
             << "1. Metode Tabel\n"
             << "2. Metode Biseksi\n"
             << "3. Metode Regula Falsi\n"
             << "4. Metode Newton-Raphson\n"
             << "5. Ganti Persamaan\n"
             << "0. Keluar\n"
             << "=======================================\n";

        cout << "Masukkan pilihan: ";

        if (!(cin >> pilihan)) {
            cout << "Input harus berupa angka!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1:
                a = inputAngka("Batas bawah (a): ");
                b = inputAngka("Batas atas (b): ");

                do {
                    step = inputAngka("Step (h): ");
                    if (step <= 0)
                        cout << "Step harus lebih besar dari 0!\n";
                } while (step <= 0);

                metodeTabel(a, b, step);
                break;

            case 2:
                a = inputAngka("Batas bawah (a): ");
                b = inputAngka("Batas atas (b): ");
                metodeBiseksi(a, b);
                break;

            case 3:
                a = inputAngka("Batas bawah (a): ");
                b = inputAngka("Batas atas (b): ");
                metodeRegulaFalsi(a, b);
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

