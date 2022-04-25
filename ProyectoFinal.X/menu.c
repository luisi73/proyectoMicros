//========  FUNCIONES MENU  ========================
void menuIntro(void)
{
    LATCSET = 0x180;
    putsUART("\n== Menu del Sistema ==\n");

    putsUART("1.Modificar PIN\n");
    putsUART("2.Mostrar usuarios\n");
    putsUART("3.Salir\n");
    // Para poder pasar a la selección de las opciones del menu
    select_option = 1; //<- Descomentar para activar para que al escribir caracteres pasemos al otro
}

void selectOption(char s[])
{
    // Debemos de verificar lo que se le pasa al micro como funcion y despues
    char s_c[0];
    int option;
    s_c[0] = s[0];
    // Select option para que no vaya a verif
    option = charToInt(s_c[0]);

    if (option == 1)
    {
        modifyPin();
    }
    else if (option == 2)
    {
        mostarUsuarios();
    }
    else if (option == 3)
    {
        putsUART("\nSaliendo del Menu...\n");
        select_option = 0;
    }
    else
    {
        putsUART("\nOpcion Incorrecta. Saliendo del Menu");
        select_option = 0;
    }
};

void mostarUsuarios(void)
{
    putsUART("\nUsuarios del sistema:\n");
    int i;
    for (i = 0; i < sizeof(nombres_pines) / sizeof(nombres_pines[0]); i++)
    {
        putsUART(nombres_pines[i]);
        putsUART("\n");
    }
}

void modifyPin(void)
{
    putsUART("Usuarios del sistema:\n");
    int i;
    char text[2];
    for (i = 0; i < sizeof(nombres_pines) / sizeof(nombres_pines[0]); i++)
    {
        sprintf(text, "%d", i + 1);
        putsUART(text);
        putsUART(". ");
        putsUART(nombres_pines[i]);
        putsUART("\n");
    }
    putsUART("\n¿De que usuario quiere cambiar el PIN?\n");
    select_user = 1;
    select_option = 0;
};

void selectUser(char s[])
{
    // Reseteamos para que no se vuelva a meter en la funci�n.
    select_user = 0;
    user_selected = charToInt(s[0]);
    putsUART("INSERTAR NUEVO PIN\nDeben de tener longitud 5");
    select_pin = 1;
    // Recibimos el nombre de 4 caracteres
}

void changePin(char s[])
{
    putsUART(s);
    strcpy(pines_acceso[user_selected - 1], s);
    putsUART("PIN CAMBIADO");
    select_pin = 0;
    select_option = 0;
    select_user = 0;
}

// Apuntes pendientes aqui
int charToInt(char c)
{
    if (c - 48 < 10)
        return c - 48;
    else if (c > 64 && c < 71)
        return c - 55;
    else
        return -1;
}