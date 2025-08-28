#!/usr/bin/env python3

import grpc


async def test_get(app_client):
    app_client.Insert("oaoa", "yaya")

    response = app_client.Get("oaoa")

    assert response == "yaya"


async def test_not_found(app_client):
    response = app_client.Get("oaoa")

    assert response == (grpc.StatusCode.NOT_FOUND, 'failed to get value from memory storage: key oaoa not found in storage')


async def test_update(app_client):
    app_client.Insert("oaoa", "yaya")
    app_client.Update("oaoa", "blabla")

    response = app_client.Get("oaoa")

    assert response == "blabla"


async def test_update_not_found(app_client):
    response = app_client.Update("oaoa", "yaya")

    assert response[0] == grpc.StatusCode.NOT_FOUND


async def test_remove(app_client):
    app_client.Insert("oaoa", "yaya")
    app_client.Remove("oaoa")

    response = app_client.Get("oaoa")

    assert response[0] == grpc.StatusCode.NOT_FOUND


async def test_remove_not_found(app_client):
    response = app_client.Remove("oaoa")

    assert response[0] == grpc.StatusCode.NOT_FOUND
