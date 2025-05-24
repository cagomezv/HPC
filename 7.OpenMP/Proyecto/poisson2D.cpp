// Simulación de la ecuación de Poisson en 2D utilizando diferencias finitas
// Usa condiciones de frontera de Dirichlet y un término fuente con forma gaussiana

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>

const double x0 = 0.0, xf = 1.0;
const double y0 = 0.0, yf = 1.0;
const double e = 8.85e-12; // Permitividad eléctrica
const double TOL = 1e-6;

// Inicializa las matrices de la grilla y las condiciones de frontera
void initialize_grid(int M, int N, std::vector<std::vector<double>>& T, std::vector<std::vector<double>>& source, double& h, double& k) {
    h = (xf - x0) / M;
    k = (yf - y0) / N;

    T.resize(M + 1, std::vector<double>(N + 1, 0.0));
    source.resize(M + 1, std::vector<double>(N + 1, 0.0));

    // Condiciones de frontera
    for (int j = 0; j <= N; ++j) {
        T[M][j] = 0.0;
        T[0][j] = 0.0;
    }
    for (int i = 0; i <= M; ++i) {
        T[i][0] = 0.0;
        T[i][N] = std::pow(x0 + i * h, 1.0);
    }
}

// Calcula el término fuente como una distribución gaussiana
void poisson_source(int M, int N, std::vector<std::vector<double>>& source, double h, double k) {
    double mu = 0.5, sigma = 0.1;
    for (int i = 0; i <= M; ++i) {
        for (int j = 0; j <= N; ++j) {
            double x = x0 + i * h;
            double y = y0 + j * k;
            source[i][j] = std::exp(-((x - mu)*(x - mu) + (y - mu)*(y - mu)) / (2 * sigma * sigma)) / (std::sqrt(2 * M_PI * sigma));

            // Condiciones de contorno para la fuente
            if (i == 0 || i == M || j == 0 || j == N) {
                source[i][j] = 0.0;
            }
        }
    }
}

// Resuelve la ecuación de Poisson iterativamente hasta convergencia
void solve_poisson(std::vector<std::vector<double>>& T, const std::vector<std::vector<double>>& source, int M, int N, double h, double k) {
    double constante = 1.0 / e;
    double delta = 1.0;

    while (delta > TOL) {
        delta = 0.0;
        for (int i = 1; i < M; ++i) {
            for (int j = 1; j < N; ++j) {
                double T_new = (
                    ((T[i + 1][j] + T[i - 1][j]) * k * k) +
                    ((T[i][j + 1] + T[i][j - 1]) * h * h) -
                    (constante * source[i][j] * h * h * k * k)) /
                    (2.0 * (h * h + k * k));

                delta = std::max(delta, std::abs(T_new - T[i][j]));
                T[i][j] = T_new;
            }
        }
    }
}

// Exporta los resultados de la matriz T a un archivo .dat
void export_to_file(const std::vector<std::vector<double>>& T, double h, double k, int M, int N, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo para escritura." << std::endl;
        return;
    }
    for (int i = 0; i <= M; ++i) {
        for (int j = 0; j <= N; ++j) {
            double x = x0 + i * h;
            double y = y0 + j * k;
            file << x << "\t" << y << "\t" << T[i][j] << "\n";
        }
    }
    file.close();
    std::cout << "Resultados exportados a " << filename << std::endl;
}

int main() {
    int M = 50, N = 50;
    double h, k;
    std::vector<std::vector<double>> T, source;

    initialize_grid(M, N, T, source, h, k);
    poisson_source(M, N, source, h, k);
    solve_poisson(T, source, M, N, h, k);

    export_to_file(T, h, k, M, N, "solucion_poisson.dat");

    std::cout << "Simulación completada." << std::endl;
    return 0;
}
