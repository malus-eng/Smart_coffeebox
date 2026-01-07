
# 3D Printed Enclosure Parts

This folder contains the 3D printable components for the device enclosure.  
All files are provided in **.STL format**, ready for slicing and printing.

## Included Parts

| File name | Description |
|----------|-------------|
| `box_base.stl` | Main body of the enclosure, holds all electronics |
| `box_separator.stl` | Internal divider separating sensor & electronics space |
| `box_lid.stl` | Top cover with cutouts, allows airflow and sensor exposure |

> File names may differ depending on your naming — adjust as needed.

---

## Recommended Print Settings
These settings are suggestions based on successful test prints:

- **Material:** PLA or PETG  
- **Layer height:** 0.2 mm  
- **Top/Bottom layers:** 4  
- **Wall count / Perimeters:** 3  
- **Infill:** 15–20% (Grid or Gyroid)  
- **Supports:** Not required  
- **Bed adhesion:** Brim optional  
- **Nozzle diameter:** 0.4 mm

---

## Assembly Notes
- The separator fits into the base without screws.
- The lid is friction-fit; can be secured using tape or small clips if desired.
- Ensure airflow openings are not obstructed (important for gas sensors).
- Sensors should be placed **above** fan and away from wind flow where possible.
- Cable lengths should allow lid to open for maintenance.

---

## CAD Format / Editing
If you wish to modify the enclosure:

- Original model was designed in **Fusion 360**
- All measurements align with:
  - Arduino MKR WAN 1310 footprint
  - 40×40×10 mm DC fan
  - Standard breadboard clearance

Source CAD files available upon request (or will be added later).

---

## Printing Orientation (Important)

| Part | Print Orientation |
|------|------------------|
| Base | Face down, flat on bed |
| Divider | Flat on side |
| Lid | Flat on top surface |

This ensures:
- Minimal supports
- Strong layer lines
- Clean tolerances

---

## License
All CAD files are distributed under the **MIT License**, same as project code.  
You may modify and print freely.

---

Happy printing! 🛠️
