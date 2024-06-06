# fusiondirectory-plugins-access

This is a plugin for FusionDirectory for managing configuration of Acces Management system such LemonLDAP::NG

## Why this plugin ?

For example, LemonLDAP::NG have a web interface to setup some application to protect trough CAS / OpendIDC / SAML
But it couldn't be easy to setup application trough models, get some dashboard or delegated to other

Trough dedicated API, FusionDirectory could setup application remotely

## What does it do ?

You could create attributes configuration (used to setup remote application)
You could create a remote Access Service (Only LemonLDAP::NG service supported)
You could define SAML/OIDC/CAS applications and apply to a remote Access Service
You could import remote SAML configuration (trough Federation Metadata) in order to import some of them

## Dependencies
This plugin need taxonomy plugin because all config parameters is stored inside a taxonomy table. It's useful if settings of remote service are modified during software evolutions.

## Todo
  - Create dashboard
  - Import remote config
  - Create SAML app configuration from Federated application (like RENATER or EDUGAIN)

## How to install

`git https://github.com/gallak/fusiondirectory-plugins-access.git
mv fusiondirectory-plugins-access access`

### usage

see screenshoot

![screen of notes](example.png)


