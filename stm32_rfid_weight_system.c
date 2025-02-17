#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "hx711.h"   // HX711 weight sensor library
#include "rfid.h"    // RFID RC522 library
#include <stdio.h>

/* ---------------------- GLOBAL VARIABLES ---------------------- */
#define ITEM_WEIGHT 500  // Example: Each item weighs 500g

QueueHandle_t rfidQueue;      // Queue to store scanned RFID tags
SemaphoreHandle_t weightSem;  // Semaphore to synchronize weight and RFID processing

int weight = 0;               // Global variable to store weight reading
int previousWeight = 0;       // Stores last weight measurement
int rfidCount = 0;            // Stores the number of scanned RFID tags

/* ---------------------- FUNCTION PROTOTYPES ---------------------- */
void WeightTask(void *pvParameters);
void RFIDTask(void *pvParameters);
void VerificationTask(void *pvParameters);
void PrintResult(int isValid);

/* ---------------------- MAIN FUNCTION ---------------------- */
int main(void) {
    /* Initialize hardware components */
    SystemInit();     // Initialize STM32 system
    HX711_Init();     // Initialize weight sensor
    RFID_Init();      // Initialize RFID sensor

    /* Create FreeRTOS Semaphore */
    weightSem = xSemaphoreCreateBinary();

    /* Create Queue for storing RFID data */
    rfidQueue = xQueueCreate(10, sizeof(int)); // Queue can hold up to 10 RFID tag IDs

    /* Create FreeRTOS Tasks */
    xTaskCreate(WeightTask, "WeightTask", 256, NULL, 2, NULL);
    xTaskCreate(RFIDTask, "RFIDTask", 256, NULL, 2, NULL);
    xTaskCreate(VerificationTask, "VerificationTask", 256, NULL, 1, NULL);

    /* Start Scheduler */
    vTaskStartScheduler();

    /* Infinite loop (should never reach here) */
    while (1);
}

/* ---------------------- TASKS IMPLEMENTATION ---------------------- */

/**
 * Task 1: Weight Sensor Task
 * - Runs continuously to measure weight
 * - Detects if weight has changed
 * - Saves new weight and signals the RFID task via semaphore
 */
void WeightTask(void *pvParameters) {
    while (1) {
        /* Read weight from HX711 */
        weight = HX711_Read();
        
        /* Check if weight has changed */
        if (weight != previousWeight) {
            previousWeight = weight;

            /* Signal the RFID task that weight has changed */
            xSemaphoreGive(weightSem);
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));  // Delay 500ms before next read
    }
}

/**
 * Task 2: RFID Reader Task
 * - Waits for weight change signal
 * - Scans RFID tags as they pass through the gate
 * - Stores RFID tag count in queue
 */
void RFIDTask(void *pvParameters) {
    int tagID;
    
    while (1) {
        /* Wait until weight has changed */
        if (xSemaphoreTake(weightSem, portMAX_DELAY) == pdTRUE) {
            rfidCount = 0; // Reset RFID count

            /* Scan for RFID tags */
            while (RFID_Available()) {
                tagID = RFID_Read(); // Read RFID tag ID

                /* Store tag ID in queue */
                xQueueSend(rfidQueue, &tagID, portMAX_DELAY);
                
                rfidCount++;  // Increment count
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));  // Delay 500ms before next scan
    }
}

/**
 * Task 3: Verification Task
 * - Fetches stored RFID data
 * - Checks if weight change matches number of scanned tags
 * - Prints result to console
 */
void VerificationTask(void *pvParameters) {
    int storedTagID;
    
    while (1) {
        /* Check if RFID data is available */
        if (uxQueueMessagesWaiting(rfidQueue) > 0) {
            int expectedWeightChange = rfidCount * ITEM_WEIGHT;
            int actualWeightChange = weight - previousWeight;

            /* Verify if the weight change matches the expected RFID count */
            if (actualWeightChange == expectedWeightChange) {
                PrintResult(1); // Print success
            } else {
                PrintResult(0); // Print error
            }

            /* Clear Queue */
            while (uxQueueMessagesWaiting(rfidQueue) > 0) {
                xQueueReceive(rfidQueue, &storedTagID, portMAX_DELAY);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 1 second before next check
    }
}

/**
 * PrintResult Function
 * - Prints success or error message to console
 */
void PrintResult(int isValid) {
    if (isValid) {
        printf("Transaction OK - Weight and RFID match.\n");
    } else {
        printf("Error Detected - Mismatch between weight and RFID tags!\n");
    }
}
