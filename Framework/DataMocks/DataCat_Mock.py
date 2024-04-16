from typing import List, Dict, Any
import datetime
import random
import json


def generate_random_date(start: datetime = datetime.datetime(2020, 1, 1), end: datetime = datetime.datetime.now()) -> str:
    """
    Generate a random datetime between two datetime objects.

    Parameters:
    start (datetime): lower limit for date generation.
    end (datatime): upper limit for date generation.

    Returns:
        A datetime object.
    """

    return start + (end - start) * random.random()


def save_logs_to_file(logs: List[Dict[str, Any]], filename: str) -> None:
    """
    Save a list of log entries to a TXT file.

    Parameters:
        logs (List[Dict[str, Any]]): The list of log entries to save.
        filename (str): The name of the file to save the logs to.

    Returns:
        None
    """

    with open(filename, "w") as f:
        for log in logs:
            f.write(json.dumps({"log": log}) + "\n")


def mock_datacat_logs(num_logs: int) -> List[Dict[str, Any]]:
    """
    Simulate the DataCat log extraction service.

    Parameters:
        num_logs (int) : The number of logs to be generated.

    Returns:
        A list of dictionaries, each representing a log entry with various details.
    """

    log_types = ["audit", 
                 "user_behavior", 
                 "failure_notification", 
                 "debug"]
    log_events = ["User 123 is logged in.", 
                  "User 456 authentication failed.", 
                  "User 789 accessed component X.",
                  "Critical failure in component Y.", 
                  "User 321 performed action Z.", 
                  "Severe damage to the rebimboca da parafuseta."]
    log_targets = ["User interface", 
                   "Database connector", 
                   "Authentication module", 
                   "Data processing unit", 
                   "Network adapter", 
                   "Storage manager",
                   "Security module"]
    log_stimulus = ["Button click", 
                    "Form submission", 
                    "Page load", 
                    "Data entry", 
                    "System boot", 
                    "File upload",
                    "Dropdown selection"]
    
    logs = []

    for _ in range(num_logs):
        log_entry = {
            "date": generate_random_date().isoformat(),
            "type": random.choice(log_types),
            "message": f"Sample log message for type {random.choice(log_events)}"
        }

        if log_entry["type"] == "audit":
            log_entry.update({
                "user_id": random.randint(1, 100),
                "action": f"Performed action {random.randint(1, 10)}"
            })
        
        if log_entry["type"] == "user_behavior":
            log_entry.update({
                "user_id": random.randint(1, 100),
                "stimulus": f"Stimulus {random.choice(log_stimulus)}",
                "target_component": f"Component {random.choice(log_targets)}"
            })
        
        if log_entry["type"] == "failure_notification":
            log_entry.update({
                "target_component": f"Component {random.choice(log_targets)}",
                "code_file_line": f"File {random.randint(1, 10)}, Line {random.randint(1, 100)}",
                "severity": random.choice(["low", "medium", "high"])
            })

        logs.append(log_entry)
    
    return logs


if __name__ == "__main__":
    # Mock data generation
    datacat_logs = mock_datacat_logs(num_logs=10)
    #print(json.dumps(datacat_logs, indent=2))
    
    # Save the logs to a file
    save_logs_to_file(datacat_logs, "datacat_logs.txt")
