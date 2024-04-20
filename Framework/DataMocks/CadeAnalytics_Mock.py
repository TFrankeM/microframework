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


def save_logs_to_http_requests(requests: List[Dict[str, List[Dict[str, Any]]]], filename: str) -> None:
    """
    Save a list of log entries as simulated HTTP requests to a TXT file.

    Parameters:
        requests (List[Dict[str, List[Dict[str, Any]]]]): The list of log entries to save.
        filename (str): The name of the file to save the logs to.

    Returns:
        None
    """

    with open(filename, "w") as f:
        for request in requests:
            for event in request["request"]:
                date = event.get("date", "") 
                f.write("POST /log HTTP/1.1\n")
                f.write("Host: localhost:8000\n")
                f.write(f"Date: {date}\n")  
                f.write("Content-Type: application/json\n")
                f.write(f"Content-Length: {len(json.dumps(event))}\n\n")
                f.write(json.dumps(event) + '\n\n')


def mock_cadeanalytics_events(num_requests: int) -> List[Dict[str, List[Dict[str, Any]]]]:
    """
    Simulate the CadêAnalytics user behavior monitoring service.

    Parameters:
        num_requests (int): Number of simulated HTTP requests to be 
        generated, each containing multiple events.

    Returns:
        A list where each element is a dictionary where each value is a list of 
        dictionaries, each representing a user behavior event.
    """
    
    event_stimulus = ["Button click", 
                      "Form submission", 
                      "Page load", 
                      "Data entry", 
                      "System boot", 
                      "File upload",
                      "Dropdown selection"]
    event_targets = ["User interface", 
                     "Database connector", 
                     "Authentication module", 
                     "Data processing unit", 
                     "Network adapter", 
                     "Storage manager",
                     "Security module"]
    requests = []

    for _ in range(num_requests):
        num_events = random.randint(1, 10)
        events = []

        for i in range(num_events):
            event = {
                "date": generate_random_date().isoformat(),
                "user_id": random.randint(1, num_requests),
                "stimulus": f"Stimulus {random.choice(event_stimulus)}",
                "target_component": f"Component {random.choice(event_targets)}"
            }
            events.append(event)
            
        requests.append({"request": events})
            
    return requests

if __name__ == "__main__":
    # Mock data generation
    cadeanalytics_requests = mock_cadeanalytics_events(num_requests=10)
    #print(json.dumps(cadeanalytics_requests, indent=2))

    # Save the logs to a file in simulated HTTP request format
    save_logs_to_http_requests(cadeanalytics_requests, "cadeanalytics_events.txt")
