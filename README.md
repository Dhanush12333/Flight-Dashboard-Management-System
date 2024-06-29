# Flight-Dashboard-Management-System

To implement an in-air flight-management platform for an air-traffic control (ATC) system. It includes a digital dashboard that maintains in-air flights bucketed in 60-minute intervals based on expected time of arrival (ETA). The project includes functions to insert new flight-plans, cancel flight-plans for emergency situations, show status of a particular flight-plan, and show flight-plans in 1-hour time periods from a given time.

## Approach
1. Data Structures Used:
   * Circular Doubly Linked List for Buckets: This was chosen due to the dynamic nature of the bucket lists caused by the rearrangement of flight-plans in 1-hour periods.
   * Singly Linked List for Flights: Used to maintain the list of flight-plans within each bucket.
2. Additionally, a TIME structure is defined to represent time in the system. This structure consists of two integer fields, min for minutes and hour for hours, allowing for accurate representation and manipulation of time-related data within the digital dashboard for the ATC system.

## Working 

