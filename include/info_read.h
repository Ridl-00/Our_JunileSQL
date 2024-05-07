#pragma once 
#ifndef INFO_READ_H_
#define INFO_READ_H_

#include <btree.h>
#include <stdio.h>
#include <time.h>

static void read_time(struct tm *time_info);

static bool read_bool(const char *prompt);

static void read_cpc_info(Feature_info *info);

static void read_cylc_info(Feature_info *info);

static void read_p_cpc_info(Feature_info *info);

void read_feature_info(StudentRecord *record);
#endif
