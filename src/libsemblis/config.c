#include <stdio.h>
#include <assert.h>
#include <wchar.h>
#include <string.h>

#include "logger.h"
#include "config.h"
#include "environment.h"
#include "utils.h"
#include "macros.h"

static char default_config_file[] = "semblis.cfg";

static void consume_comment(FILE * fin) {
    int c = fgetc(fin);
    while(! feof(fin)) {
        char cc = c;
	
        if(cc == '\n')
            return;

        c = fgetc(fin);
    }
}

static void consume_whitespace(FILE *fin) {
    while(! feof(fin)) {
        int c = fgetc(fin);
        char cc = c;

        if(cc != ' ' && cc != '\t' && cc != '\n') {
            ungetc(c, fin);
            return;
        }
    }
}

#define MAX_KEYWORD_SIZE   64
#define MAX_VALUE_SIZE   64

Section *prev_section = NULL;

static void consume_line(Config *conf, FILE * fin)
{
    char keyword[MAX_KEYWORD_SIZE];
    uint32 keyword_len = 0;
    char value[MAX_VALUE_SIZE];
    uint32 value_len = 0;
    int c;
    char cc;
    Section *new_section;

    c = fgetc(fin);
    cc = c;

    if(cc == '[') {
        /* section header */
        cc = fgetc(fin);
        while(cc != ']') {
            keyword[keyword_len++] = cc;
            cc = fgetc(fin);
        }

        keyword[keyword_len++] = 0;

        new_section = section_create(keyword);
	
        conf->_sections = util_grow_buffer_to_size(conf->_sections,
                                                   &conf->_capacity,
                                                   conf->_num_sections+1,
                                                   sizeof(*conf->_sections));


        conf->_sections[conf->_num_sections++] = new_section;

        prev_section = new_section;
	
        return;
    } else {
        while(cc != ' ' && cc != '\t' && cc != '=' && !feof(fin)) {
            keyword[keyword_len++] = cc;
            cc = fgetc(fin);
        }

        keyword[keyword_len++] = 0;

        if(cc != '=') {
            consume_whitespace(fin);
	    
            cc = fgetc(fin);
            if(cc != '=')
                return;
	    
            consume_whitespace(fin);
        } else {
            consume_whitespace(fin);
        }

        cc = fgetc(fin);
        while(cc != '\n' && !feof(fin)) {
            value[value_len++] = cc;
            cc = fgetc(fin);
        }

        value[value_len++] = 0;

        assert(prev_section);

        section_set_prop(prev_section, keyword, value);

        /*
          printf("new property in section: %s :: %s ==> %s\n",
          section_name(prev_section),
          keyword, value);
        */
    }
}

Property *property_create(char *key, char *val)
{
    Property *prop = malloc(sizeof(*prop));
    prop->key = malloc(sizeof(*key) * (strlen(key) + 1));
    strcpy(prop->key, key);
    prop->value = malloc(sizeof(*val) * (strlen(val) + 1));
    strcpy(prop->value, val);    
    return prop;
}

void property_destroy(Property *prop)
{
    free(prop->value);
    free(prop->key);
    free(prop);
}

Section *section_create(char * name)
{
    Section *sec = malloc(sizeof(*sec));

    sec->_name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(sec->_name, name);

    sec->_properties = NULL;
    sec->_properties_cap = 0;
    sec->_properties_num = 0;

    return sec;
}

void section_destroy(Section *section)
{
    int i;

    assert(section);

    for(i=0; i<section->_properties_num; i++)
        property_destroy(section->_properties[i]);
    
    free(section->_name);
    free(section);
}

char * section_name(Section *section)
{
    return section->_name;
}

void section_set_prop(Section *section, char *key, char *value)
{
    Property *prop = property_create(key, value);
    
    section->_properties = util_grow_buffer_to_size(section->_properties,
                                                    &section->_properties_cap,
                                                    section->_properties_num+1,
                                                    sizeof(*section->_properties));

    section->_properties[section->_properties_num++] = prop;
}

int section_get_num_properties(Section *section)
{
    return section->_properties_num;
}

Property *section_get_property(Section *section, int prop_num)
{
    assert(prop_num < section->_properties_num);

    return section->_properties[prop_num];
}

Config *config_create(char * name)
{
    size_t name_len = strlen(name) + 1;
    Config *conf = malloc(sizeof(*conf));

    conf->_name = malloc(name_len);
    strcpy(conf->_name, name);
    
    conf->_sections = NULL;
    conf->_num_sections = 0;
    conf->_capacity = 0;

    return conf;
}

void config_destroy(Config *conf)
{
    int i;

    for(i=0; i<conf->_num_sections; i++)
        section_destroy(conf->_sections[i]);

    free(conf->_sections);

    free(conf->_name);
    free(conf);
}

void config_read_default_config(Config *conf)
{
    config_read_config(conf, default_config_file);
}

void config_read_config(Config *conf, const char *name)
{
    int c;
    FILE* fin = fopen(name, "r");

    if(fin == NULL) {
        char error[512];
        snprintf(error, 512, "config file not found: %s\n", name);
        logger_log("config", LL_WARNING, error);
        return;
    }

    c = fgetc(fin);
    while(! feof(fin)) {
        char cc = c;
	
        consume_whitespace(fin);

        if(cc == '#') {
            consume_comment(fin);
        } else {
            ungetc(c, fin);

            consume_whitespace(fin);
	    
            consume_line(conf, fin);
            
            consume_whitespace(fin);
        }

        consume_whitespace(fin);

        c = fgetc(fin);
    }

    fclose(fin);
}

Section **config_get_section_list(Config *conf)
{
    return  conf->_sections;
}
