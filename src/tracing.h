#pragma once

#include "perfetto.h"

namespace sft {

constexpr const char* kTraceCategoryRasterizer = "rasterizer";
constexpr const char* kTraceCategoryTest = "rasterizer";
constexpr const char* kTraceCategoryApplication = "application";

void StartTracing();

void StopTracing();

}  // namespace sft

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category(sft::kTraceCategoryRasterizer)  //
        .SetDescription("Events from the rasterizer."),
    perfetto::Category(sft::kTraceCategoryApplication)  //
        .SetDescription("Events from the application."),
    perfetto::Category(sft::kTraceCategoryTest)  //
        .SetDescription("Events from the test harness."));
