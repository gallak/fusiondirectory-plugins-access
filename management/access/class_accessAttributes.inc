<?php
/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2016-2017  FusionDirectory
  Copyright (C) 2021  Antoine Gallavardin

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */


 
class accessAttributes extends simplePlugin
{

  protected $displayHeader  = TRUE;
  protected $loaded = FALSE;



  static function plInfo (): array
  {
    return [
      'plShortName'   => _('attributes'),
      'plDescription' => _('attributes'),
      'plObjectClass' => ['fdAccessAttributes'],
      'plObjectType'  => ['attributes' => [
        'name'        => _('attributes'),
        'icon'        => 'geticon.php?context=applications&icon=attributes&size=16',
        'ou'          => get_ou('AccessAttributesRDN'),
        'mainAttr'    => 'cn',
      ]],
      'plSearchAttrs'  => ['description'],

      'plProvidedAcls'  => parent::generatePlProvidedAcls(static::getAttributesInfo())
    ];
  }

  static function getAttributesInfo (): array
  {
    return [
      'attributes' => [
        'name'  => _('Information'),
        'attrs' => [
                new StringAttribute(
                  _('Name'), _('Name of Attributes'),
                  'cn', TRUE),
                new StringAttribute(
                  _('description '), _('description'),
                  'description', TRUE),
                new StringAttribute(
                  _('OID'), _('OID code'),
                  'fdAccessAttributeOID', TRUE),
                new StringAttribute(
                  _('SAML FriendlyName'), _('FriendlyName'),
                  'fdAccessAttributeFriendlyName', TRUE),
                  new StringAttribute(
                    _('OIDC Claims'), _('Claims'),
                    'fdAccessAttributeClaims', TRUE),
                  ]
      ]
    ];
  }


  function __construct ($dn = NULL, $object = NULL, $parent = NULL, $mainTab = FALSE)
  {
    global $config;
    parent::__construct($dn, $object, $parent, $mainTab);
  }


  function compute_dn (): string
  {
    global $config;
    return ("cn=".$this->cn.",".get_ou('AccessAttributesRDN').$config->current['BASE']);
  }
}
