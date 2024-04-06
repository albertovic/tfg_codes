#include <iostream>
#include <vector>
#include <sstream>

std::vector<double> dividirMensaje(const char *mensaje) {
    std::vector<double> resultado;
    std::stringstream ss(mensaje); // Creamos un stringstream con el mensaje

    std::string parte;
    while (ss >> parte) { // Leemos cada parte del stringstream
        double valor;
        if (std::istringstream(parte) >> valor) { // Intentamos convertir la parte en un double
            // Si la conversión es exitosa, agregamos el valor al vector resultado
            resultado.push_back(valor);
        }
    }

    return resultado;
}

int main() {
    const char *mensaje = "t 0.2 0.3";
    std::vector<double> valores = dividirMensaje(mensaje);

    // Imprimimos los valores almacenados en el vector
    for (double valor : valores) {
        std::cout << valor << std::endl;
    }

    return 0;
}