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


 
class samlEntity extends simplePlugin
{

  protected $displayHeader  = TRUE;
  protected $loaded = FALSE;


  static function getFederationContent (): array 
  {
    return(array());
  }


  static function plInfo (): array
  {
    return [
      'plShortName'   => _('IDP / SP'),
      'plDescription' => _('IDP/SP'),
      'plObjectClass' => ['fdAccessIdpSp'],
      'plObjectType'  => ['samlEntity' => [
        'name'        => _('samlEntity'),
        'icon'        => 'geticon.php?context=applications&icon=federation&size=16',
        'ou'          => get_ou('federationRDN'),
        'mainAttr'    => 'cn',
      ]],
      'plSearchAttrs'  => ['description'],

      'plProvidedAcls'  => parent::generatePlProvidedAcls(static::getAttributesInfo())
    ];
  }

  static function getAttributesInfo (): array
  {
    return [
      'Entity' => [
        'name'  => _('Information'),
        'attrs' => [
                new StringAttribute(
                  _('EntityID'), _('EntityID'),
                  'cn', TRUE),
                new StringAttribute(
                  _('Federation'), _('Url of Federation'),
                  'fdAccessEntityAuthority', TRUE),
                new SelectAttribute(
                    _('type'), _('Type'),
                    'fdAccessEntityType', TRUE,["IDP","SP"],"IDP",["IDP","SP"]),
                new StringAttribute(
                  _('Organization'), _('Organization'),
                  'o', TRUE),
                new StringAttribute(
                  _('Technical Contact'), _('Technical Contact'),
                  'mail', TRUE),
                  new StringAttribute(
                    _('Description'), _('Description'),
                    'description', FALSE),
        ]
      ],
    ];
  }


  function __construct ($dn = NULL, $object = NULL, $parent = NULL, $mainTab = FALSE)
  {
    global $config;
    parent::__construct($dn, $object, $parent, $mainTab);
  }

}
