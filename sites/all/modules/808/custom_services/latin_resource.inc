<?php

function latin_news_index(){
    /*
    $query = db_select('node' , 'node');
    $query->leftJoin('field_data_body', 'body', 'body.entity_id = node.nid');
    $query->leftJoin('field_data_field_image', 'image', 'image.entity_id = node.nid');
    $query->join('file_managed', 'file', 'file.fid = image.field_image_fid');
    $query->fields('node' , array('nid' , 'title' , 'uid' , 'created' , 'changed'));
    $query->fields('body' , array('body_value'));
    $query->fields('file' , array('filename' , 'uri'));
    $query->condition('node.type', 'article');
    $query->condition('node.language', 'en');
    $query->condition('node.status', 1);
    $query->condition('body.entity_type', 'node');
    $query->condition('body.bundle', 'article');
    $query->condition('image.entity_type', 'node');
    $query->condition('image.bundle', 'article');
    */

    $query = db_select('node' , 'node');
    $query->leftJoin('field_data_field_eventtime', 'eventtime', 'eventtime.entity_id = node.nid');
    $query->leftJoin('field_data_field_company', 'company', 'company.entity_id = node.nid');
    $query->leftJoin('field_data_field_ifwebinar', 'ifwebinar', 'ifwebinar.entity_id = node.nid');
    $query->fields('node' , array('nid' , 'title'));
    $query->fields('eventtime' , array('field_eventtime_value'));
    $query->fields('company' , array('field_company_value'));
    $query->fields('ifwebinar' , array('field_ifwebinar_value'));
    $query->condition('node.type', 'article');
    $query->condition('node.language', 'en');
    $query->condition('node.status', 1);
    $query->condition('eventtime.field_eventtime_value', date("Y-m-d", time()) , '>=');
    $nodes = $query->execute()->fetchAll();
    return $nodes;
}