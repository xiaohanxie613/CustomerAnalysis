# 🧠 Marketing Campaign Analysis Tool

A C++ program that imports and analyzes marketing campaign data from a CSV file using an SQLite3 database. This tool provides detailed insights on customer spending behavior, campaign effectiveness, and predictive patterns based on demographics like income, family size, and marital status.

## 📊 Features

- Imports customer marketing data from a tab-separated CSV file.
- Automatically cleans and standardizes input (e.g., filters invalid marital statuses).
- Creates and populates an SQLite3 database (`marketing.db`) with structured data.
- Provides an interactive **menu-driven interface** with the following analyses:
  1. **Top Customer Spenders** – View top N spenders with ID and total spending.
  2. **Spending Analysis** – Breakdown of spending by income group, family size, and marital status.
  3. **Campaign Effectiveness** – Measures effectiveness of five marketing campaigns.
  4. **Predictive Spending Behavior** – Displays expected average spending by predicted group (Low, Mid, High).
  5. **Exit** – Closes the tool gracefully.

## 🔭 Future Plans

- ** Search:** Add a search feature to isolate specific individuals or categories
- **Data Visualization:** Add graphs or charts (e.g., using Python or a C++ GUI wrapper) to display trends visually.
- **Export Results:** Enable exporting analysis results to CSV or formatted reports.
- **Machine Learning Integration:** Train a lightweight model (outside of C++) to predict customer response likelihood to campaigns.
- **Dynamic CSV Input:** Allow the user to specify or upload different CSV files at runtime.

