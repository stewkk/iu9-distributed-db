#!/usr/bin/env python3

import pathlib
import sys

import pytest

import Client

pytest_plugins = ['testsuite.pytest_plugin']


def pytest_addoption(parser):
    group = parser.getgroup('app service')
    group.addoption(
        '--app-service-port',
        help='Bind app service to this port (default is %(default)s)',
        default=8080,
        type=int,
    )


@pytest.fixture
async def app_service(
    ensure_daemon_started,
    # Service process holder
    app_service_scope,
    # Service dependencies
    mockserver,
):
    # Start service if not started yet
    await ensure_daemon_started(app_service_scope)


@pytest.fixture
async def app_client(
    app_service,
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
