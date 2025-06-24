#ifndef CONFIG_H
#define CONFIG_H

struct H2OFilter {
    int ventil_pumpe;
    int hex_generator;
    int filter_gpd;
    int filter_max_menge_filtern;
    int filter_verhaeltnis;
    int filter_clean_time;
    int filter_zeit_100ml;
    float filter_total_liter;
};

struct H2OTank {
    int abwassertank_vorhanden;
    int abwassertank_groesse;
    float abwassertank_total_liter;
};

struct Configuration {
    H2OFilter filter;
    H2OTank tank;
};

#endif  // CONFIG_H
