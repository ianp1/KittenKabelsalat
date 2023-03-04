#include <Arduino.h>

//Number of readings to be aggregated before using them
#define CONNECTOR_READ_MAX 5

//Number of consecutive readings that have to be the same in order to qualify as being stable
#define CONNECTOR_STABLE_READINGS_NUM 3


class Connector {
    private:
        gpio_num_t pin;
        int connectorNumber;
        long valueSum;
        int evaluatedValue;
        int lastEvaluatedValue;
        int stableCount;
        int readCount;

    public:
        Connector(gpio_num_t pin, int connectorNumber) : 
            pin(pin),
            connectorNumber(connectorNumber), 
            valueSum(0), 
            evaluatedValue(-1), 
            lastEvaluatedValue(-1), 
            stableCount(0), 
            readCount(0) 
        {

        }
        
        /**
         * @brief Is connector being stable over a time period?
         * 
         * Returns if the averaged values stayed the same for the last @see CONNECTOR_STABLE_READINGS_NUM.
         * If yes, we can assume that there really is a connection present, and that value is accurate
         * 
         * @return true if readings stayed stable during time period
         * @return false otherwise
         */
        bool isStable() {
            return stableCount >= CONNECTOR_STABLE_READINGS_NUM;
        }


        /**
         * @brief Get currently evaluated value
         * 
         * Value is averaged over @see CONNECTOR_READ_MAX measurements
         * 
         * @return int 
         */
        int getValue() {
            return evaluatedValue;
        }

        void loop() {
            valueSum += analogRead(this -> pin);

            this -> readCount ++;

            if (this -> readCount >= CONNECTOR_READ_MAX) {
                this -> lastEvaluatedValue = this -> evaluatedValue;

                double currentValue = ((valueSum/(double)(readCount)) - 150.0) / 302.0;
                if (currentValue > -0.03) {
                    evaluatedValue = (int) round(currentValue);
                } else {
                    evaluatedValue = -1;
                }

                this -> readCount = 0;
                this -> valueSum = 0;

                if (this -> evaluatedValue == this -> lastEvaluatedValue && this -> evaluatedValue >= 0) {
                    this -> stableCount ++;

                    if (this -> stableCount >= CONNECTOR_STABLE_READINGS_NUM) {
                        this -> stableCount = CONNECTOR_STABLE_READINGS_NUM;


                        Serial.print("stable reading: ");
                        Serial.print(connectorNumber + 1);
                        Serial.print(" connected to connector ");
                        Serial.println(evaluatedValue + 1);
                    }
                } else {
                    this -> stableCount = 0;
                }
            }
        }

};