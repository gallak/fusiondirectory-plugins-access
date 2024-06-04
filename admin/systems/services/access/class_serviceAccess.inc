<?php

/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2012-2016  FusionDirectory
  Copyright (C) 2021-2023  Antoine Gallavardin

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

class serviceAccess extends simpleService
{
  static function plInfo (): array
  {
    return [
      'plShortName'   => _('Access Server'),
      'plDescription' => _('Access Server').' ('._('Services').')',
      'plIcon'        => 'geticon.php?context=applications&icon=netdisco&size=16',
      'plObjectClass' => ['fdAccessService'],

      'plProvidedAcls'  => parent::generatePlProvidedAcls(static::getAttributesInfo())
    ];
  }



  public function checkservice (){

      $obj = "accessMethod".$this->fdAccessServiceType;
      $cnx=new $obj;
      $cnxInfo = array("base_url" => $this->fdAccessServiceURL,"username" => $this->fdAccessServiceUser, "password" => $this->fdAccessServicePassword);
      $res=$cnx->checkStatus($cnxInfo);
      if ( $res['code'] == "200" ) {
        msg_dialog::display(_('Acces LEMONLDAP'), "MESSAGE :". $res['data']['name']. " === " .$res['data']['status'] ,INFO_DIALOG);
      }else{
        msg_dialog::display(_('Acces LEMONLDAP'), "MESSAGE :" .$res['msg'] ,WARNING_DIALOG);
      }    
  }




  /*!
   *  \brief The main function : information about attributes
   */
  static function getAttributesInfo (): array
  {
    return  [
      'main' => [
        'name'  => _('Access service'),
        'attrs' => [
          new StringAttribute(
            _('URL'), _('URL to access the Access server'),
            'fdAccessServiceURL', TRUE
          ),
          new StringAttribute(
            _('User'), _('User to access access server API.'),
            'fdAccessServiceUser', FALSE
          ),
          new PasswordAttribute(
            _('Password'), _('Password to access access server API.'),
            'fdAccessServicePassword', FALSE
          ),
          new SelectAttribute(
            _('Type of AccessService'), _('Type of Acces Service'),
            'fdAccessServiceType', FALSE, [],'',[]
          ),
          new ButtonAttribute(_('check service'), _('Check service'),
              'fdAccessServiceCheck', _('Check'),'checkservice'
          ),
        ]
      ],
    ];
  }

  static public function get_methods ()
  {
    global $class_mapping;
    $available=array();
    foreach ($class_mapping as $class => $path) {
      if (preg_match('/^accessMethod.+/', $class)) {
        $obj = new $class;
        $available[$obj->getMethodCode()] = $obj->getMethodName();
      }
    }
    return $available;
  }


  function __construct ($dn = NULL, $object = NULL, $parent = NULL, $mainTab = FALSE)
  {
    parent::__construct($dn, $object, $parent, $mainTab);

    $allMethods=$this->get_methods();
    $serviceType_id=array_keys($allMethods);
    $serviceType_val=array_values($allMethods);


  

    $this->attributesAccess['fdAccessServiceType']->setChoices($serviceType_id,$serviceType_val);

  }


}
