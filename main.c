#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

sensor *init_sensor(int n)
{
    sensor *sensors = malloc(sizeof(sensor) * n);
    return sensors;
}

// print senzor cu indexul dat in comanda
void print_sensor_data(sensor s)
{
    if (s.sensor_type == TIRE) {// printare tip TIRE
    tire_sensor *tire_data = (tire_sensor *)s.sensor_data;
    printf("Tire Sensor\n");
    printf("Pressure: %.2f\n", tire_data->pressure);
    printf("Temperature: %.2f\n", tire_data->temperature);
    printf("Wear Level: %d%%\n", tire_data->wear_level);
    if (tire_data->performace_score == 0)
    printf("Performance Score: Not Calculated\n");
    else
    printf("Performance Score: %d%%\n", tire_data->performace_score);
    } else if (s.sensor_type == PMU) {// printare tip PMU
    power_management_unit *pmu_data = (power_management_unit *)s.sensor_data;
    printf("Power Management Unit\n");
    printf("Voltage: %.2f\n", pmu_data->voltage);
    printf("Current: %.2f\n", pmu_data->current);
    printf("Power Consumption: %.2f\n", pmu_data->power_consumption);
    printf("Energy Regen: %d%%\n", pmu_data->energy_regen);
    printf("Energy Storage: %d%%\n", pmu_data->energy_storage);
    }
}
// sortare vector de senzori dupa prioritati
sensor *sort_sensors(sensor *sensors, int n)
{
    sensor *s = malloc(n * sizeof(sensor));
    int count = 0;
    for (int i = 0; i < n; i++) {
    if (sensors[i].sensor_type == PMU) {
    s[count] = sensors[i];
    count++;
    }
    }
    for (int i = 0; i < n; i++) {
    if (sensors[i].sensor_type == TIRE) {
    s[count] = sensors[i];
    count++;
    }
    }
    return s;
}

// creare vector de senzori fara date eronate
sensor *filter_sensors(sensor *sensors, int n_sensors)
{
    sensor *filtered_sensors = malloc(n_sensors * sizeof(sensor));
    int n_filtered_sensors = 0;
    for (int i = 0; i < n_sensors; i++) {
    if (sensors[i].sensor_type == TIRE) {
    tire_sensor *tire_data = (tire_sensor *) sensors[i].sensor_data;
    if (tire_data->pressure >= 19 && tire_data->pressure <= 28 &&
    tire_data->temperature >= 0 && tire_data->temperature <= 120 &&
    tire_data->wear_level >= 0 && tire_data->wear_level <= 100) {
    filtered_sensors[n_filtered_sensors] = sensors[i];
    n_filtered_sensors++;
    }
    } else if (sensors[i].sensor_type == PMU) {
power_management_unit *pmu_data =
 (power_management_unit *)sensors[i].sensor_data;
    if (pmu_data->voltage >= 10 && pmu_data->voltage <= 20 &&
    pmu_data->current >= -100 && pmu_data->current <= 100 &&
    pmu_data->power_consumption >= 0 && pmu_data->power_consumption <= 1000 &&
    pmu_data->energy_regen >= 0 && pmu_data->energy_regen <= 100 &&
    pmu_data->energy_storage >= 0 && pmu_data->energy_storage <= 100) {
    filtered_sensors[n_filtered_sensors] = sensors[i];
    n_filtered_sensors++;
    }
    }
    }
    return filtered_sensors;
}

// eliberare PMU
void free_power_management_unit(power_management_unit *pmu)
{
    free(pmu);
}

// eliberare TIRE
void free_tire_sensor(tire_sensor *tire)
{
    free(tire);
}

// eliberare vector de senzori
void free_sensors(sensor *s, int n)
{
    int i;
    for (i = 0; i < n; i++) {
    if (s[i].sensor_type == TIRE) {
    free_tire_sensor((tire_sensor *)s[i].sensor_data);
    } else if (s[i].sensor_type == PMU) {
    free_power_management_unit((power_management_unit *)s[i].sensor_data);
    }
    free(s[i].operations_idxs);
    }
    free(s);

}

int main(int argc, char const *argv[])
{
    FILE *fbin;
    fbin = fopen(argv[1], "rb");

    if (fbin == NULL) {
    printf("Nu s-a putut deschide fisierul\n");
    exit(1);
    }
    // citire datele despre senzori
    int nr_sensors, i, j;
    fread(&nr_sensors, sizeof(int), 1, fbin);
    sensor *sensors = init_sensor(nr_sensors);
    // citire date pt fiecare sensor si salvarea lor intr-un vector
    for (i = 0; i < nr_sensors; i++) {
    fread(&sensors[i].sensor_type, sizeof(enum sensor_type), 1, fbin);
    if (sensors[i].sensor_type == PMU) {
    sensors[i].sensor_data = malloc(sizeof(power_management_unit));
fread(&((power_management_unit *)sensors[i].sensor_data)->voltage,
      sizeof(float), 1, fbin);
fread(&((power_management_unit *)sensors[i].sensor_data)->current,
      sizeof(float), 1, fbin);
fread(&((power_management_unit *)sensors[i].sensor_data)->power_consumption,
      sizeof(float), 1, fbin);
fread(&((power_management_unit *)sensors[i].sensor_data)->energy_regen,
      sizeof(int), 1, fbin);
fread(&((power_management_unit *)sensors[i].sensor_data)->energy_storage,
      sizeof(int), 1, fbin);
fread(&sensors[i].nr_operations, sizeof(int), 1, fbin);
sensors[i].operations_idxs = malloc(sizeof(int) * sensors[i].nr_operations);
for (j = 0; j < sensors[i].nr_operations; j++)
    fread(&sensors[i].operations_idxs[j], sizeof(int), 1, fbin);
    }
    if (sensors[i].sensor_type == TIRE) {
    sensors[i].sensor_data = malloc(sizeof(tire_sensor));
fread(&((tire_sensor *)sensors[i].sensor_data)->pressure,
sizeof(float), 1, fbin);
fread(&((tire_sensor *)sensors[i].sensor_data)->temperature,
sizeof(float), 1, fbin);
fread(&((tire_sensor *)sensors[i].sensor_data)->wear_level,
sizeof(int), 1, fbin);
fread(&((tire_sensor *)sensors[i].sensor_data)->performace_score,
 sizeof(int), 1, fbin);
fread(&sensors[i].nr_operations, sizeof(int), 1, fbin);
sensors[i].operations_idxs = malloc(sizeof(int) * sensors[i].nr_operations);
for (j = 0; j < sensors[i].nr_operations; j++)
    fread(&sensors[i].operations_idxs[j], sizeof(int), 1, fbin);
    }
    }
    char command[100];
    int nr, ok = 0;
    sensor *sortSensors;
    sortSensors = sort_sensors(sensors, nr_sensors);
    sensor *sensor_clear;
   while (fgets(command, sizeof(command), stdin) != NULL) {
    if (strncmp(command, "print", 5) == 0) {
    sscanf(command, "print %d", &nr);
    if (nr < 0 || nr > nr_sensors) {
    printf("Index not in range!\n");
    } else if (ok == 0)
    print_sensor_data(sortSensors[nr]);
    else
    print_sensor_data(sensor_clear[nr]);
    } else if (strcmp(command, "clear\n") == 0) {
    ok = 1;
    sensor_clear = filter_sensors(sortSensors, nr_sensors);
    } else if (strcmp(command, "exit\n") == 0) {
    // comanda de exit, eliberare memorie
    free_sensors(sensors, nr_sensors);
    break;
    }
    }
    // inchiderea fisierului
    fclose(fbin);
    return 0;
}