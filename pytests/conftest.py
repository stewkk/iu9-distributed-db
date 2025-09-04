#!/usr/bin/env python3

import pathlib
import sys
import shutil
import os

import pytest

import Client


def pytest_addoption(parser):
    group = parser.getgroup('db-service')
    group.addoption(
        '--app-service-port',
        help='Bind app service to this port (default is %(default)s)',
        default=8080,
        type=int,
    )


@pytest.fixture
async def app_client(
    app_service_port,
):
    # Create service client instance
    return Client.Client("localhost", app_service_port)


@pytest.fixture(scope='session')
def app_service_port(pytestconfig):
    return f'{pytestconfig.option.app_service_port}'


@pytest.fixture(scope='session')
def app_root():
    """Path to app service root."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
async def app_service_scope(
    pytestconfig,
    create_daemon_scope,
    app_root,
):
    async with create_daemon_scope(
        args=[
            str(app_root.joinpath('build', 'bin', 'iu9-db')),
            '--port',
            str(pytestconfig.option.app_service_port),
        ],
        health_check=Client.Client("localhost", f'{pytestconfig.option.app_service_port}').Healthcheck,
    ) as scope:
        yield scope


@pytest.fixture(autouse=True)
def clear_db(app_client):
    app_client.Clear()
    shutil.rmtree('/tmp/iu9-distributed-db')
    os.mkdir('/tmp/iu9-distributed-db')
    yield
