#pragma once

#include "perfetto.h"

namespace sft {

constexpr const char* kTraceCategoryRasterizer = "rasterizer";
constexpr const char* kTraceCategoryTest = "rasterizer";

}  // namespace sft

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category(sft::kTraceCategoryRasterizer)  //
        .SetDescription("Events from the rasterizer."),
    perfetto::Category(sft::kTraceCategoryTest)  //
        .SetDescription("Events from the test harness."));
