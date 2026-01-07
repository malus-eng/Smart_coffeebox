# 3D Printed Enclosure Parts

This folder contains the 3D printable components for the device enclosure.  
All files are provided in **.STL format** and are ready to be imported into a slicing tool.

## Included Parts

| File name            | Description |
|---------------------|-------------|
| `box_base.stl`       | Main enclosure body, supports and houses all electronics |
| `box_separator.stl`  | Internal divider separating the sensor chamber, coffee bean storage compartment, and electronics area |
| `box_lid.stl`        | Top lid with a recessed slot designed to hold a desiccant pack |

---

## Recommended Print Settings

The following parameters are suggested based on successful test prints:

- **Material:** PLA or PETG  
- **Layer height:** 0.2 mm  
- **Top/Bottom layers:** 4  
- **Walls / Perimeters:** 3  
- **Infill:** 15–20% (Grid or Gyroid recommended)  
- **Supports:** Enable automatic supports  
- **Bed adhesion:** Brim recommended if parts lift  
- **Nozzle diameter:** 0.4 mm

---

## Assembly Notes

- The separator slots into the base; for stability, applying cyanoacrylate (502 glue) at the joints is recommended.
- The lid is friction-fit and can be removed easily.
- Remove all support material after printing and ensure no ventilation slots are blocked, as airflow is critical for fan operation.
- Sensors should be placed away from direct airflow from the fan.
- Wire routing should allow the lid to open without disconnecting components.

---

## CAD Format and Modifications

If you would like to modify the enclosure:

- The parts were designed in **Fusion 360**
- Dimensions are aligned to:
  - Arduino MKR WAN 1310 footprint
  - 40×40×10 mm DC fan
  - Breadboard and wiring clearance

Feel free to modify and re-export as needed.

---

## Recommended Print Orientation

| Part     | Print Orientation |
|----------|------------------|
| Base     | Flat, bottom face down |
| Divider  | Laid flat on print bed |
| Lid      | Flat, top surface down |

Printing in these orientations helps ensure:

- Minimal support usage
- Stronger layer adhesion
- Reliable fit and tolerances

---

## License

All enclosure files are released under the same **MIT License** as the project code.  
You are free to print, modify, and redistribute these models.

---

Happy printing! 🛠️

