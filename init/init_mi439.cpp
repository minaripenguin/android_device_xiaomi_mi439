/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/file.h>
#include <filesystem>

#include <libinit_sdm439.h>

using std::filesystem::directory_iterator;
using std::filesystem::filesystem_error;

static const variant_info_t pine_info = {
    .brand = "Xiaomi",
    .device = "pine",
    .marketname = "",
    .model = "Redmi 7A",
};

static const variant_info_t olive_info = {
    .brand = "Xiaomi",
    .device = "olive",
    .marketname = "",
    .model = "Redmi 8",
};

static void set_acdb_path_props(std::string device)
{
    int i = 0;
    try {
        for (const auto& acdb : directory_iterator("/vendor/etc/acdbdata/" + device + "/"))
            property_override(("persist.vendor.audio.calfile" + std::to_string(i++)).c_str(), acdb.path().c_str());
    } catch (const filesystem_error&) {
        // Ignore
    }
}

static void set_props_olive()
{
    /* Camera */
    property_override("persist.vendor.camera.aec.sync", "1");
    property_override("persist.vendor.camera.awb.sync", "2");
    property_override("persist.vendor.camera.expose.aux", "1");

    /* Charger */
    property_override("persist.vendor.ctm.disallowed", "true");
}

static void determine_device()
{
    std::string fdt_model;
    android::base::ReadFileToString("/sys/firmware/devicetree/base/model", &fdt_model, true);
    if (fdt_model.find("PINE QRD") != fdt_model.npos) {
        set_variant_props(pine_info);
        set_acdb_path_props("pine");
    } else if (fdt_model.find("Olive QRD") != fdt_model.npos) {
        set_variant_props(olive_info);
        set_acdb_path_props("olive");
        set_props_olive();
    }
}

void vendor_load_properties() {
    set_dalvik_heap_size();
    determine_device();
}
