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
 
bool esFechaValida(const string& fecha) {
    if (fecha.length() != 10) return false;
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
 
bool esHoraValida(const string& hora) {
    // TODO
    return false;
}
 
int dividirLinea(const string& linea, string campos[], int maxCampos) {
    // TODO
    return 0;
}
 
unsigned long hashString(const string& s) {
    // TODO
    return 0;
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