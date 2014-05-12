static char* menu_main_headers[2] = {"Select direction", "Information"};

struct {
    char title[2][30];
    char subtitle[2][30];
}direction_menu[2] = {
    {
        {"From Office", "To Office"},
        {"BC Protasov -> Subway", "Subway -> BC Protasov"}
    },
    {
        {"Usage", "About"},
        {"",""}
    }
};

struct {
    char* content;
    GTextAlignment text_align;
}information[2] = {
    {
        "\nPress central button to search for next bus departure.\n\nPress again to search in other schedule section.",
        GTextAlignmentLeft
    },
    {
        "\nGlobalLogic Kiev Office Shuttle Bus Schedule v.0.1 (2014-05-12)\n\n2014 Alexei Koliada",
        GTextAlignmentCenter
    }
};

struct {
    char* header[30];
    char* time[2][20];
    int size[2];
}schedule_array[2] = {
    {
        {"To 'Palats Ukraina'", "To 'Druzhby Narodiv'"},
        {
            {"11:45", "12:45", "13:45", "14:45", "15:45", "17:05", "17:35", "18:00", "18:10", "18:15", "18:30", "18:40", "19:00", "19:20", "19:40", "20:35", "21:00"},
            {"18:50", "20:05", "21:30"}
        },
        {17, 3}
    },
    {
        {"From 'Palats Ukraina'", "From 'Druzhby Narodiv'"},
        {
            {"7:45", "8:20", "8:40", "8:50", "9:00", "9:10", "9:30", "9:40", "9:50", "10:00", "10:10", "10:20", "10:30", "11:00", "11:30", "12:00", "13:00", "14:00", "15:00", "16:00"},
            {"8:45", "9:35"}
        },
        {20, 2}
    }
};