from dateutil.relativedelta import relativedelta
from typing import List, Dict, Any
import datetime
import random
import csv

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


def save_logs_to_csv(spreadsheets: Dict[str, List[Dict[str, Any]]], filename: str) -> None:
    """
    Save a list of log entries to a CSV file.

    Parameters:
        spreadsheets (Dict[str, List[Dict[str, Any]]]): The list of log entries to save.
        filename (str): The name of the file to save the logs to.

    Returns:
        None
    """

    with open(filename, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        
        # Write users section
        writer.writerow(["users"])
        writer.writerow(["user_id", "name", "surname", "address", "registration_date", "birthday"])
        for entry in spreadsheets["users"]:
            writer.writerow([entry.get("user_id", ""), entry.get("name", ""), entry.get("surname", ""), entry.get("address", ""), entry.get("registration_date", ""), entry.get("birthday", "")])
        
        # Write products section
        writer.writerow(["products"])
        writer.writerow(["product_id", "name", "image", "description", "price"])
        for entry in spreadsheets["products"]:
            writer.writerow([entry.get("product_id", ""), entry.get("name", ""), entry.get("image", ""), entry.get("description", ""), entry.get("price", "")])
        
        # Write inventory section
        writer.writerow(["inventory"])
        writer.writerow(["product_id", "quantity"])
        for entry in spreadsheets["inventory"]:
            writer.writerow([entry.get("product_id", ""), entry.get("quantity", "")])
        
        # Write orders section
        writer.writerow(["orders"])
        writer.writerow(["user_id", "product_id", "quantity", "creation_date", "payment_date", "delivery_date"])
        for entry in spreadsheets["orders"]:
            writer.writerow([entry.get("user_id", ""), entry.get("product_id", ""), entry.get("quantity", ""), entry.get("creation_date", ""), entry.get("payment_date", ""), entry.get("delivery_date", "")])


def mock_contaverde_data(num_spreadsheets: int) -> Dict[str, List[Dict[str, Any]]]:
    """
    Simulate the ContaVerde ERP system data.

    Parameters:
        num_spreadsheets (int): The number of spreadsheets to be generated for each type of data.
    
    Returns:
        A dictionary with keys for each data type (users, products, inventory, orders)
        and values as lists of dictionaries representing each item or transaction.
    """
    spreadsheet_names = ["Andres", 
                         "Harry", 
                         "Shakira", 
                         "Alexandre", 
                         "Thanos", 
                         "Annabeth", 
                         "Deide"]
    spreadsheet_surname = ["D'Alessandro", 
                           "Potter", 
                           "Mebarak", 
                           "de Moraes", 
                           "Pereira", 
                           "Chase", 
                           "Costa"]
    spreadsheet_address = ["Rua das Flores, 123", 
                           "Av. Paulista, 456", 
                           "Praca da Se, 789", 
                           "Alameda Santos, 321", 
                           "Rua Augusta, 654", 
                           "Av. Brasil, 987", 
                           "Praca da Liberdade, 111"]
    spreadsheet_description = ["Smartphone com tela de 6.5''", 
                               "Notebook i7, 16GB RAM", 
                               "Geladeira 400L, inox, frost-free",
                               "TV 50'', 4K, Smart", 
                               "Ar-condicionado 12000 BTUs", 
                               "Maquina de lavar 11kg", 
                               "Micro-ondas 30L, inox"]

    users = [{"user_id": i,
              "name": random.choice(spreadsheet_names),
              "surname": random.choice(spreadsheet_surname),
              "address": random.choice(spreadsheet_address),
              "registration_date": generate_random_date().isoformat(),
              "birthday": generate_random_date(start=datetime.datetime(1950, 1, 1), end=datetime.datetime.now() - relativedelta(years=18)).isoformat()} 
              for i in range(1, num_spreadsheets)]
    
    products = [{"product_id": i,
                 "name": f"Product{i}",
                 "image": f"image{i}.png",
                 "description": random.choice(spreadsheet_description),
                 "price": round(random.uniform(10.0, 500.0), 2)} 
                 for i in range(1, num_spreadsheets)]
    
    inventory = [{"product_id": product["product_id"], 
                  "quantity": random.randint(0, 100)} 
                  for product in products]
    
    orders = []
    for i in range(1, num_spreadsheets):
        creation_date = generate_random_date(datetime.datetime.strptime(users[i-1]["registration_date"], "%Y-%m-%dT%H:%M:%S.%f"), datetime.datetime(2024, 1, 1))
        payment_date = generate_random_date(creation_date, creation_date + relativedelta(days=10))
        delivery_date = generate_random_date(payment_date, payment_date + relativedelta(days=30))
        orders.append({
            "user_id": i,
            "product_id": i,
            "quantity": random.randint(1, inventory[i-1]["quantity"]),
            "creation_date": creation_date.isoformat(),
            "payment_date": payment_date.isoformat(),
            "delivery_date": delivery_date.isoformat()
    })

    return {"users": users,
            "products": products,
            "inventory": inventory,
            "orders": orders}

if __name__ == "__main__":
    # Mock data generation
    contaverde_data = mock_contaverde_data(num_spreadsheets=10)
    
    # Save the spreadsheets to a CSV file
    save_logs_to_csv(contaverde_data, "contaverde_spreadsheets.csv")