<?php
/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)

  Copyright (C) 2012-2020 FusionDirectory

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

class dcimConfig extends simplePlugin
{
  static function plInfo (): array
  {
    return [
      'plShortName'     => _('Access configuration'),
      'plDescription'   => _('FusionDirectory Access plugin configuration'),
      'plObjectClass'   => ['fdAccessPluginConf'],
      //'plCategory'      => ['configuration'],
      'plObjectType'    => ['configuration'],
      'plPriority'      => 10,
      'plProvidedAcls'  => parent::generatePlProvidedAcls(static::getAttributesInfo())
    ];
  }

  static function getAttributesInfo (): array
  {
    return [
      'access' => [
        'name'  => _('ACCESS'),
        'attrs' => [
          new StringAttribute(
            _('Federation RDN'), _('Branch in which Federation details info will be stored'),
            'fdAccessFederationRDN', TRUE,
            'ou=federation'
          ),
          new StringAttribute(
            _('Attribute RDN'), _('Branch in which Attributes details info will be stored'),
            'fdAccessAttributesRDN', TRUE,
            'ou=attributes'
          ),
          new StringAttribute(
            _('Attribute RDN'), _('Referential used for fetching dedicated settings'),
            'fdAccessConfigReferential', TRUE,
            'ACCESS'
          ),
        ]
      ],
    ];
  }
}
