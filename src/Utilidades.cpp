#include <iostream>
#include <string>
using namespace std;
#pragma once
 
// ============================================================
//  Utilidades.cpp  -  no depende de ningún otro archivo
// ============================================================
 
const string ESTADO_PENDIENTE = "Pendiente";
const string ESTADO_TRANSITO  = "EnTransito";
const string ESTADO_ENTREGADO = "Entregado";
const string ESTADO_CANCELADO = "Cancelado";
 
long long calcularClaveFecha(const string& fecha, const string& hora) {
    long long total = 0;

    int i = 0;
    int largo = fecha.length();
    while (i < largo) {
        if (fecha[i] >= '0' && fecha[i] <= '9') {
            total = total * 10 + (fecha[i] - '0');
        }
        i++;
    }

    i = 0;
    largo = hora.length();
    while (i < largo) {
        if (hora[i] >= '0' && hora[i] <= '9') {
            total = total * 10 + (hora[i] - '0');
        }
        i++;
    }
    return total;
}
 
long long claveInicioDia(const string& fecha) {
    return calcularClaveFecha(fecha, "00:00:00");
}
 
long long claveFinDia(const string& fecha) {
    return calcularClaveFecha(fecha, "23:59:59");
}
 
// todos los datos se ven en el csv los numeros tambien por si hay dudas de porque hay 10 u 8 caracteres;
bool esFechaValida(const string& fecha) {
    if (fecha.length() != 10) return false; // Formato esperado: YYYY-MM-DD
    if (fecha[4] != '-' || fecha[7] != '-') return false;

    int i = 0;
    while (i < 10) {
        if (i == 4 || i == 7) {
            if (fecha[i] < '0' || fecha[i] > '9') return false;
        }
        i++;
    }
    
    i = 4;    
    int año = (fecha[0] - '0') * 1000 + (fecha[1] - '0') * 100 + (fecha[2] - '0') * 10 + (fecha[3] - '0');
    int mes = (fecha[5] - '0') * 10 + (fecha[6] - '0');
    int dia = (fecha[8] - '0') * 10 + (fecha[9] - '0');

    if (año < 1900 || año > 2100) return false;
    if (mes < 1 || mes > 12) return false;
    if (dia < 1 || dia > 31) return false;

    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) return false;
    
    if (mes == 2) {
        bool esBisiesto = (año % 4 == 0 && año % 100 != 0) || (año % 400 == 0);
        if (esBisiesto && dia > 29) return false;
        if (!esBisiesto && dia > 28) return false;
    }
    return true;
}
 
// mismo formato de la funcion esFechaValida, pero para la hora
bool esHoraValida(const string& hora) {
    if (hora.length() != 8) return false;  // Formato esperado: HH:MM:SS
    if (hora[2] != ':' || hora[5] != ':') return false; // esto comprueba que los : esten en la posicion correcta

    int i = 0;
    while (i < 8) { 
        if (i == 2 || i == 5) {
            if (hora[i] < '0' || hora[i] > '9') return false;
        }
        i++;
    }

    int horaNum = (hora[0] - '0') * 10 + (hora[1] - '0');
    int minutoNum = (hora[3] - '0') * 10 + (hora[4] - '0');
    int segundoNum = (hora[6] - '0') * 10 + (hora[7] - '0');

    if (horaNum < 0 || horaNum > 23) return false;
    if (minutoNum < 0 || minutoNum > 59) return false;
    if (segundoNum < 0 || segundoNum > 59) return false;

    return true;
}
 
int dividirLinea(const string& linea, string campos[], int maxCampos) {
    int contador = 0;
    string espaciado = "";
    int i = 0;
    int largo = linea.length();

    while (i < largo && contador < maxCampos) {
        if (linea[i] == ',') {
            campos[contador] = espaciado;
            contador++;
            espaciado = "";
        } else {
            espaciado += linea[i];
        }
        i++;
    }
    if (contador < maxCampos) {
        campos[contador] = espaciado;
        contador++;
    }
    return contador;
}
 
unsigned long hashString(const string& s) {
    unsigned long total = 0;
    int i = 0;
    int largo = s.length();
    while (i < largo) {
        total = total * 31 + s[i];  // 31 salvador :)
        i++;
    }
    return total;
}

int textoAEntero(const string& s) {
    int n = 0;
    int i = 0;
    int largo = s.length();
    while (i < largo) {
        if (s[i] >= '0' && s[i] <= '9') {
            n = n * 10 + (s[i] - '0');
        }
        i++;
    }
    return n;
}

double textoADecimal(const string& s) {
    double n = 0;
    double divisor = 1;
    bool despuesDelPunto = false;
    int i = 0;
    int largo = s.length();
    while (i < largo) {
        if (s[i] == '.') {
            despuesDelPunto = true;
        } else if (s[i] >= '0' && s[i] <= '9') {
            n = n * 10 + (s[i] - '0');
            if (despuesDelPunto) divisor = divisor * 10;
        }
        i++;
    }
    return n / divisor;
}