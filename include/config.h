#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "types.h"

typedef struct {
    char *key;
    char *value;
} Property;

Property *property_create(char *key, char *val);
void property_destroy(Property *prop);

typedef struct {
    char * _name;
    Property **_properties;
    unsigned int _properties_cap;
    int _properties_num;
} Section;

Section *section_create(char * name);
void section_destroy(Section *section);

char * section_name(Section *section);
void section_set_prop(Section *section, char *key, char *value);
int section_get_num_properties(Section *section);
Property *section_get_property(Section *section, int prop_num);


typedef struct {
    Section **_sections;
    unsigned int _capacity;
    int _num_sections;
    char * _name;
} Config;

/* make static/private
    void consumeLine(FILE * fin);
static const std::string& DefaultConfigFile;
*/

Config *config_create(char * name);
void config_destroy(Config *conf);

void config_read_default_config(Config *conf);
void config_read_config(Config *conf, const char *name);
Section **config_get_section_list(Config *conf);

#endif	// __CONFIG_H__
