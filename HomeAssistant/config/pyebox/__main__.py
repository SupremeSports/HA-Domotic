import asyncio
import argparse
import json
import sys

from pyebox import EboxClient, REQUESTS_TIMEOUT, PyEboxError


def _format_output(account, data):
    """Format data to get a readable output"""
    data['account'] = account
    output = ("""Ebox data for account: {d[account]}

Balance
=======
Balance:      {d[balance]:.2f} $

Usage
=====
Usage:      {d[usage]:.2f} %

Before offpeak
==============
Download: {d[before_offpeak_download]:.2f} Gb
Upload:   {d[before_offpeak_upload]:.2f} Gb
Total:    {d[before_offpeak_total]:.2f} Gb

Offpeak
=======
Download: {d[offpeak_download]:.2f} Gb
Upload:   {d[offpeak_upload]:.2f} Gb
Total:    {d[offpeak_total]:.2f} Gb

Total
=====
Download: {d[download]:.2f} Gb
Upload:   {d[upload]:.2f} Gb
Total:    {d[total]:.2f} Gb
Limit:    {d[limit]:.2f} Gb
""")
    print(output.format(d=data))


def main():
    """Main function"""
    parser = argparse.ArgumentParser()
    parser.add_argument('-u', '--username',
                        required=True, help='EBox account')
    parser.add_argument('-p', '--password',
                        required=True, help='Password')
    parser.add_argument('-j', '--json', action='store_true',
                        default=False, help='Json output')
    parser.add_argument('-t', '--timeout',
                        default=REQUESTS_TIMEOUT, help='Request timeout')
    args = parser.parse_args()
    client = EboxClient(args.username, args.password, args.timeout)

    loop = asyncio.get_event_loop()
    task = loop.create_task(client.fetch_data())
    try:
        loop.run_until_complete(task)
    except PyEboxError as exp:
        print(exp)
        client.close_session()
        return
    if not client.get_data():
        client.close_session()
        return
    if args.json:
        print(json.dumps(client.get_data()))
    else:
        _format_output(args.username, client.get_data())
    client.close_session()


if __name__ == '__main__':
    sys.exit(main())