#!/usr/bin/env python3

async def test_get(app_client):
    app_client.Insert("oaoa", "yaya")

    response = app_client.Get("oaoa")

    assert response == "yaya"
