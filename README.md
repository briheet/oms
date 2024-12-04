# Order execution and management system

This repo includes a CLI tool for order execution on Deribit test.

Current Limits:

- Matching engine requests - 5/s (20/s in a burst)
- Other requests - 20/s (100/s in a burst).

Informal command workings for now:

- place - `./main place --instrument BNB_USDC --amount 1 --type market`
- cancel - `./main cancel --order_id <place_your_order_id_here>`
